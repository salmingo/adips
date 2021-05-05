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
	bkSpacePtr_ = BackStatSpace::Create(param);
	loadPreproc_= 0;
}

ADIReduce::~ADIReduce() {
}

bool ADIReduce::do_real_process() {
	// 读取图像文件头和数据
	int retCode = fitsImg_.LoadImage(frame_->filepath.c_str());
	if (retCode) {
		_gLog.Write(LOG_FAULT, "[%s]: %s", frame_->filename.c_str(),
				retCode == 1 ? "open error"
					: (retCode == 2 ? "missing keywords"
						: "data read error"));
		return false;
	}
	frame_->wImg    = fitsImg_.wImg;
	frame_->hImg    = fitsImg_.hImg;
	frame_->expdur  = fitsImg_.expdur;
	frame_->dateobs = fitsImg_.dateobs;
	// 预处理
	if (param_->preRemoveBadpix) {// 剔除坏像素

	}

	if (!loadPreproc_) {// 尝试加载预处理图像

	}
	if (loadPreproc_ == 1) {
		if (param_->pathDark.size()) {

		}
		if (param_->pathFlat.size()) {

		}
	}

	// 统计背景
	if (param_->backMode == FILTER_SPACE) {// 空域
		bkSpacePtr_->ProcImage(fitsImg_.data, frame_);
	}
	else {// 频域

	}
	// 提取信号

	// 目标聚合

	// 计算目标特征

	// 处理特殊目标

	return true;
}

void ADIReduce::remove_bad_pixels() {

}

void ADIReduce::load_preproc_images() {

}

bool ADIReduce::precheck_dimension() {
	return true;
}

void ADIReduce::preprocess_zero() {

}

void ADIReduce::preprocess_dark() {

}

void ADIReduce::preprocess_flat() {

}
