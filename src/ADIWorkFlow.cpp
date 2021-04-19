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

ADIWorkFlow::ADIWorkFlow() {
	param_   = NULL;
	combine_ = 0;
}

ADIWorkFlow::~ADIWorkFlow() {
}

bool ADIWorkFlow::Start(Parameter* param) {
	param_ = param;
	thrd_dip_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_dip, this)));
	if (param->useAstrometry || param->usePhotometry || param->useMotionAss)
		thrd_astro_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_astro, this)));
	if (param->usePhotometry || param->useMotionAss)
		thrd_photo_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_photo, this)));
	if (param->useMotionAss)
		thrd_motio_.reset(new boost::thread(boost::bind(&ADIWorkFlow::thread_motio, this)));

	return true;
}

void ADIWorkFlow::Stop() {
	interrupt_thread(thrd_dip_);
	interrupt_thread(thrd_astro_);
	interrupt_thread(thrd_photo_);
	interrupt_thread(thrd_motio_);

	dequeDIP_.clear();
	dequeAstro_.clear();
	dequePhoto_.clear();
	dequeMotio_.clear();
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

}

/* 线程接口 */
void ADIWorkFlow::thread_dip() {
	boost::mutex mtx;
	mutex_lock lck(mtx);

	while (1) {
		cv_dip_.wait(lck);

//		while (!reduct_->IsWorking() && queReduct_.size()) {
//			mutex_lock lck1(mtx_frm_reduct_);
//			FramePtr frame;
//			frame = queReduct_.front();
//			queReduct_.pop_front();
//			if (check_image(frame)) reduct_->DoIt(frame);
//		}
	}
}

void ADIWorkFlow::thread_astro() {
	boost::mutex mtx;
	mutex_lock lck(mtx);

	while (1) {
		cv_astro_.wait(lck);
	}
}

void ADIWorkFlow::thread_photo() {

}

void ADIWorkFlow::thread_motio() {

}

void ADIWorkFlow::interrupt_thread(threadptr& thrd) {
	if (thrd.unique()) {
		thrd->interrupt();
		thrd->join();
		thrd.reset();
	}
}
