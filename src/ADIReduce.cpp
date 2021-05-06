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
	/* 预处理 */
	// 剔除坏像素
	if (param_->preProc.badPixRemove) remove_bad_pixels();
	// 尝试加载预处理图像
	if (!loadPreproc_) load_preproc_images();
	if (loadPreproc_ == 1) {// 预处理
		preprocess_zero();
		if (param_->preProc.pathDark.size()) preprocess_dark();
		if (param_->preProc.pathFlat.size()) preprocess_flat();
	}
	// 统计背景
	switch (param_->backStat.mode) {
	case FILTER_SPACE:
		//		bkSpacePtr_->ProcImage(fitsImg_.data, frame_);
		break;
	case FILTER_FREQ_DOMAIN:
		break;
	default:
		break;
	}
	// 提取信号

	// 目标聚合

	// 计算目标特征

	// 处理特殊目标

	return true;
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

/*---------------------------------------------------------------------------*/
/* 接口: 坏像素 */
void ADIReduce::remove_bad_pixels() {
	_gLog.Write("removing hot and dark pixels");
	// 备份原始数据. 之后原始数据区存储处理结果, 备份区作为原始输入
	unsigned w = fitsImg_.wImg;
	unsigned h = fitsImg_.hImg;
	imgBackup_.CopyData(fitsImg_.data, w, h);
	/**
	 * 剔除坏像素. 坏像素判据:
	 * - 使用3*3邻近窗口作为邻近区
	 * - 像素值大于任一邻近值的k倍. k==3   ==> 热点
	 * - 像素值小于任一邻近值             ==> 暗点
	 */
	float *bufDst = fitsImg_.data;
	float *bufSrc = imgBackup_.buff;
	char *badMarked = new char[w * h];
	unsigned x1(1), y1(1), x2(w - 1), y2(h - 1); // 检测区域
	unsigned x, y;
	unsigned pos;
	int step;
	// 遍历检测区
	for (y = y1, pos = y1 * w; y < y2; ++y, pos += w) {
		for (x = x1; x < x2; ++x) {
			if ((step = check_bad_pixel_neighbor(badMarked, w, h, x, y))) x += step;
			else if (check_bad_pixel(bufSrc, w, h, x, y)) {
				bufDst[pos + x]    = correct_bad_pixel(bufSrc, w, h, x, y);
				badMarked[pos + x] = 1;
				++x;
			}
		}
	}

	// 释放临时存储区
	delete []badMarked;
}

bool ADIReduce::check_bad_pixel(float* data, unsigned w, unsigned h, unsigned x, unsigned y) {

	return true;
}

int ADIReduce::check_bad_pixel_neighbor(char* mask, unsigned w, unsigned h, unsigned x, unsigned y) {
	unsigned above = (y - 1) * w + x;
	if (mask[above])     return 1;  // 正上方
	if (mask[above + 1]) return 2;  // 右上方
	//...左上方
	return 0;
}

float ADIReduce::correct_bad_pixel(float* data, unsigned w, unsigned h, unsigned x, unsigned y) {
	double sum(0.0);
	unsigned pos = y * w + x;
	float t = data[pos];
	return float((sum - t) * 0.125);
}

/*---------------------------------------------------------------------------*/
