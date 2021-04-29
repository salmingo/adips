/*!
 * @class ADIReduce 天文数字图像处理. 从图像中提取目标信号并测量目标特征
 * @version 0.1
 * @date 2021-04
 */


#include "ADIReduce.h"
#include "GLog.h"

ADIReduce::ADIReduce(Parameter* param)
	: ADIProcess(param) {
	nameFunc_ = "reducing";
	maxThread_ = boost::thread::hardware_concurrency() / 2;
	if (maxThread_ == 0) maxThread_ = 1;
	_gLog.Write("thread concurrency count: %d", maxThread_);
}

ADIReduce::~ADIReduce() {
}

bool ADIReduce::do_real_process() {
	// 读取图像文件头和数据
	int retCode = hFITS_.LoadImage(frame_->filepath.c_str());
	if (retCode) {
		_gLog.Write(LOG_FAULT, "[%s]: %s", frame_->filename.c_str(),
				retCode == 1 ? "open error"
					: (retCode == 2 ? "missing keywords"
						: "data read error"));
		return false;
	}
	// 统计背景

	// 提取信号

	// 目标聚合

	// 计算目标特征

	// 处理特殊目标

	return true;
}
