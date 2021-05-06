/**
 * @class ADIWorkFlow 天文数字图像作业流程
 * @version 0.1
 * @date 2021-04
 */

#include <boost/bind/bind.hpp>
#include <boost/filesystem.hpp>
#include "ADIWorkFlow.h"
#include "GLog.h"

using namespace boost::filesystem;
using namespace boost::placeholders;

ADIWorkFlow::ADIWorkFlow(boost::asio::io_service* ios)
	: ios_(ios) {
	param_     = NULL;
	combine_   = 0;
	running_   = false;
	procCount_ = 0;
}

ADIWorkFlow::~ADIWorkFlow() {
}

bool ADIWorkFlow::Start(Parameter* param) {
	param_     = param;
	running_   = true;
	procCount_ = 0;

	const ADIReduce::CBResultSlot &slot1 = boost::bind(&ADIWorkFlow::DIReduceResult, this, _1);
	reduce_.reset(new ADIReduce(param_));
	reduce_->RegisterResult(slot1);
	thrd_reduce_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_reduce, this)));

	if (param->funcs.useAstrometry || param->funcs.usePhotometry || param->funcs.useMotion) {
		const ADIReduce::CBResultSlot &slot2 = boost::bind(&ADIWorkFlow::AstrometryResult, this, _1);
		astrometry_.reset(new AAstrometry(param_));
		astrometry_->RegisterResult(slot2);
		thrd_astro_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_astro, this)));
	}

	if (param->funcs.usePhotometry || param->funcs.useMotion) {
		const ADIReduce::CBResultSlot &slot2 = boost::bind(&ADIWorkFlow::PhotometryResult, this, _1);
		photometry_.reset(new APhotometry(param_));
		photometry_->RegisterResult(slot2);
		thrd_photo_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_photo, this)));
	}

	if (param->funcs.useMotion) {
		const ADIReduce::CBResultSlot &slot2 = boost::bind(&ADIWorkFlow::MotionResult, this, _1);
		motion_.reset(new AFindPV(param_));
		motion_->RegisterResult(slot2);
		thrd_motion_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_motion, this)));
	}

	return true;
}

void ADIWorkFlow::Stop() {
	running_ = false;

	interrupt_thread(thrd_reduce_);
	interrupt_thread(thrd_astro_);
	interrupt_thread(thrd_photo_);
	interrupt_thread(thrd_motion_);

	dequeReduce_.clear();
	dequeAstro_.clear();
	dequePhoto_.clear();
	dequeMotion_.clear();
}

void ADIWorkFlow::BeginCombine(Parameter* param, int mode) {
	param_   = param;
	combine_ = mode;
}

void ADIWorkFlow::AddCombineImage(const char* filePath) {
	vecCombine_.push_back(std::string(filePath));
}

void ADIWorkFlow::EndCombine() {

	vecCombine_.clear();
}

void ADIWorkFlow::ProcessImage(const char* filePath) {
	ImgFrmPtr frame;
	frame.reset(new ImageFrame);

	// 解析路径
	path pathFull = filePath;
	frame->filepath = filePath;
	frame->pathdir  = pathFull.parent_path().string();
	frame->filename = pathFull.filename().string();
	frame->filetit  = pathFull.stem().string();
	++procCount_;
	if (thrd_astro_.unique())  ++procCount_;
	if (thrd_photo_.unique())  ++procCount_;
	if (thrd_motion_.unique()) ++procCount_;

	// 加入队列并启动处理流程
	mutex_lock lck(mtx_frm_reduce_);
	dequeReduce_.push_back(frame);
	if (!reduce_->IsWorking()) cv_reduce_.notify_one();
}

/* 回调函数接口 */
void ADIWorkFlow::DIReduceResult(bool rslt) {
	--procCount_;
	if (rslt) {// 处理成功
		ImgFrmPtr frame = reduce_->GetFrame();
		if (param_->funcs.useAstrometry || param_->funcs.usePhotometry || param_->funcs.useMotion) {// 后续处理: 触发定位
			mutex_lock lck(mtx_frm_astro_);
			dequeAstro_.push_back(frame);
			cv_astro_.notify_one();
		}
		else OutputFrame(frame);
	}
	if (dequeReduce_.size()) {// 尝试处理缓存区中其它图像
		cv_reduce_.notify_one();
	}
	else if (!procCount_ && ios_) {// 完成处理流程, 退出程序
		ios_->stop();
	}
}

void ADIWorkFlow::AstrometryResult(bool rslt) {
	--procCount_;
	ImgFrmPtr frame = astrometry_->GetFrame();
	if (rslt && (param_->funcs.usePhotometry || param_->funcs.useMotion)) {// 后续处理: 触发测光
		mutex_lock lck(mtx_frm_photo_);
		dequePhoto_.push_back(frame);
		cv_photo_.notify_one();
	}
	else OutputFrame(frame);

	if (dequeAstro_.size()) {// 尝试处理缓存区中其它图像
		cv_astro_.notify_one();
	}
	else if (!procCount_ && ios_) {// 完成处理流程, 退出程序
		ios_->stop();
	}
}

void ADIWorkFlow::PhotometryResult(bool rslt) {
	--procCount_;
	ImgFrmPtr frame = photometry_->GetFrame();
	OutputFrame(frame);

	if (rslt && param_->funcs.useMotion) {// 后续处理: 触发运动关联
		mutex_lock lck(mtx_frm_motion_);
		dequeMotion_.push_back(frame);
		cv_motion_.notify_one();
	}
	if (dequePhoto_.size()) {// 尝试处理缓存区中其它图像
		cv_photo_.notify_one();
	}
	else if (!procCount_ && ios_) {// 完成处理流程, 退出程序
		ios_->stop();
	}
}

void ADIWorkFlow::MotionResult(bool rslt) {
	--procCount_;
	if (dequeMotion_.size()) {// 尝试处理缓存区中其它图像
		cv_motion_.notify_one();
	}
	else if (!procCount_ && ios_) {// 完成处理流程, 退出程序
		ios_->stop();
	}
}

void ADIWorkFlow::OutputFrame(ImgFrmPtr frame) {
	if (param_->output.rsltFinal) {//...输出目标测量结果
	}
	if (frame->succAstro) {//...输出天文定位结果
	}
}

/* 线程接口 */
void ADIWorkFlow::thread_reduce() {
	boost::mutex mtx;
	mutex_lock lck(mtx);

	while (running_) {
		cv_reduce_.wait(lck);

		while (running_ && !reduce_->IsWorking() && dequeReduce_.size()) {
			mutex_lock lck1(mtx_frm_reduce_);
			ImgFrmPtr frame;
			frame = dequeReduce_.front();
			dequeReduce_.pop_front();
			if (!reduce_->DoIt(frame)) --procCount_;
		}
	}
}

void ADIWorkFlow::thread_astro() {
	boost::mutex mtx;
	mutex_lock lck(mtx);

	while (running_) {
		cv_astro_.wait(lck);

		while (running_ && !astrometry_->IsWorking() && dequeAstro_.size()) {
			mutex_lock lck1(mtx_frm_astro_);
			ImgFrmPtr frame;
			frame = dequeAstro_.front();
			dequeAstro_.pop_front();
			if (!astrometry_->DoIt(frame)) --procCount_;
		}
	}
}

void ADIWorkFlow::thread_photo() {
	boost::mutex mtx;
	mutex_lock lck(mtx);

	while (running_) {
		cv_photo_.wait(lck);

		while (running_ && !photometry_->IsWorking() && dequePhoto_.size()) {
			mutex_lock lck1(mtx_frm_photo_);
			ImgFrmPtr frame;
			frame = dequePhoto_.front();
			dequePhoto_.pop_front();
			if (!photometry_->DoIt(frame)) --procCount_;
		}
	}
}

void ADIWorkFlow::thread_motion() {
	boost::mutex mtx;
	mutex_lock lck(mtx);

	while (running_) {
		cv_motion_.wait(lck);

		while (running_ && !motion_->IsWorking() && dequeMotion_.size()) {
			mutex_lock lck1(mtx_frm_motion_);
			ImgFrmPtr frame;
			frame = dequeMotion_.front();
			dequeMotion_.pop_front();
			if (!motion_->DoIt(frame)) --procCount_;
		}
	}
}

void ADIWorkFlow::interrupt_thread(threadptr& thrd) {
	if (thrd.unique()) {
		thrd->interrupt();
		thrd->join();
	}
}
