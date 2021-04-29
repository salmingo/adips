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

ADIWorkFlow::ADIWorkFlow() {
	param_   = NULL;
	combine_ = 0;
	running_ = false;
}

ADIWorkFlow::~ADIWorkFlow() {
}

bool ADIWorkFlow::Start(Parameter* param) {
	param_ = param;

	const ADIReduce::CBResultSlot &slot1 = boost::bind(&ADIWorkFlow::DIReduceResult, this, _1);
	reduce_.reset(new ADIReduce(param_));
	reduce_->RegisterResult(slot1);
	thrd_reduce_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_reduce, this)));

	if (param->useAstrometry || param->usePhotometry || param->useMotion) {
		const ADIReduce::CBResultSlot &slot2 = boost::bind(&ADIWorkFlow::AstrometryResult, this, _1);
		astrometry_.reset(new AAstrometry(param_));
		astrometry_->RegisterResult(slot2);
		thrd_astro_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_astro, this)));
	}

	if (param->usePhotometry || param->useMotion) {
		const ADIReduce::CBResultSlot &slot2 = boost::bind(&ADIWorkFlow::PhotometryResult, this, _1);
		photometry_.reset(new APhotometry(param_));
		photometry_->RegisterResult(slot2);
		thrd_photo_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_photo, this)));
	}

	if (param->useMotion) {
		const ADIReduce::CBResultSlot &slot2 = boost::bind(&ADIWorkFlow::MotionResult, this, _1);
		motion_.reset(new AFindPV(param_));
		motion_->RegisterResult(slot2);
		thrd_motion_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_motion, this)));
	}

	running_ = true;
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
	frame->pathdir = pathFull.parent_path().string();
	frame->filename = pathFull.filename().string();
	frame->filetit  = pathFull.stem().string();

	// 加入队列并启动处理流程
	mutex_lock lck(mtx_frm_reduce_);
	dequeReduce_.push_back(frame);
	cv_reduce_.notify_one();
}

/* 回调函数接口 */
void ADIWorkFlow::DIReduceResult(bool rslt) {
	_gLog.Write("ADIWorkFlow::DIReduceResult(): %s", rslt ? "Success" : "Fail");
	if (rslt) {// 处理成功
		//...在日志中记录处理结果

		// 触发后续处理
		if (param_->useAstrometry || param_->usePhotometry || param_->useMotion) {
			mutex_lock lck(mtx_frm_astro_);
			dequeAstro_.push_back(reduce_->GetFrame());
			cv_astro_.notify_one();
		}
	}
	else {//...日志记录处理结果

	}
	// 尝试处理缓存区中其它图像
	cv_reduce_.notify_one();
}

void ADIWorkFlow::AstrometryResult(bool rslt) {
	_gLog.Write("ADIWorkFlow::AstrometryResult(): %s", rslt ? "Success" : "Fail");
	if (rslt) {// 处理成功
		//...在日志中记录处理结果

		// 触发后续处理
		if (param_->usePhotometry || param_->useMotion) {
			mutex_lock lck(mtx_frm_photo_);
			dequePhoto_.push_back(astrometry_->GetFrame());
			cv_photo_.notify_one();
		}
	}
	else {//...日志记录处理结果

	}
	// 尝试处理缓存区中其它图像
	cv_astro_.notify_one();
}

void ADIWorkFlow::PhotometryResult(bool rslt) {
	_gLog.Write("ADIWorkFlow::PhotometryResult(): %s", rslt ? "Success" : "Fail");
	if (rslt) {// 处理成功
		//...在日志中记录处理结果

		// 触发后续处理
		if (param_->useMotion) {
			mutex_lock lck(mtx_frm_motion_);
			dequeMotion_.push_back(photometry_->GetFrame());
			cv_motion_.notify_one();
		}
	}
	else {//...日志记录处理结果

	}
	// 尝试处理缓存区中其它图像
	cv_photo_.notify_one();
}

void ADIWorkFlow::MotionResult(bool rslt) {
	//...在日志中记录处理结果

	// 尝试处理缓存区中其它图像
	cv_motion_.notify_one();
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
			reduce_->DoIt(frame);
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
			astrometry_->DoIt(frame);
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
			photometry_->DoIt(frame);
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
			motion_->DoIt(frame);
		}
	}
}

void ADIWorkFlow::interrupt_thread(threadptr& thrd) {
	if (thrd.unique()) {
		thrd->interrupt();
		thrd->join();
	}
}
