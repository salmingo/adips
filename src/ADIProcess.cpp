/*!
 * @class ADIProcess 天文数字图像处理基类
 * @version 0.1
 * @date 2021-04
 */

#include <boost/bind/bind.hpp>
#include "ADIProcess.h"
#include "GLog.h"

ADIProcess::ADIProcess(Parameter* param) {
	param_   = param;
	imgData_ = NULL;
	working_ = false;
}

ADIProcess::~ADIProcess() {
}


void ADIProcess::RegisterResult(const CBResultSlot &slot) {
	if (!cbRslt_.empty()) cbRslt_.disconnect_all_slots();
	cbRslt_.connect(slot);
}

bool ADIProcess::IsWorking() {
	return working_;
}

ImgFrmPtr ADIProcess::GetFrame() {
	return frame_;
}

bool ADIProcess::DoIt(ImgFrmPtr frame, float* imgData) {
	if (working_) return false;

	_gLog.Write("Start on %s [%s]", nameFunc_.c_str(), frame->filename.c_str());
	frame_   = frame;
	imgData_ = imgData;
	working_ = true;
	thrd_proc_.reset(new boost::thread(boost::bind(&ADIProcess::thread_process, this)));

	return true;
}

void ADIProcess::thread_process() {
	bool rslt = do_real_process();
	working_ = false;
	cbRslt_(rslt);
}
