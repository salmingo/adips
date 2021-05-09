/*!
 * @class ADIReduce 天文数字图像处理. 从图像中提取目标信号并测量目标特征
 * @version 0.1
 * @date 2021-04
 */

#include <math.h>
#include "ADIReduce.h"
#include "GLog.h"

ADIReduce::ADIReduce(Parameter* param)
	: ADIProcess(param) {
	nameFunc_ = "reducing";
	loadPreprocZero_ = 0;
	loadPreprocDark_ = 0;
	loadPreprocFlat_ = 0;
	buffPtr_.reset(new MemoryBuffer(param->backStat.gridWidth, param->backStat.gridHeight));
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
	// 尝试加载预处理图像
	if (!loadPreprocZero_) load_preproc_zero();
	if (!loadPreprocDark_) load_preproc_dark();
	if (!loadPreprocFlat_) load_preproc_flat();
	if (loadPreprocZero_ == 1) preprocess_zero();
	if (loadPreprocDark_ == 1) preprocess_dark();
	if (loadPreprocFlat_ == 1) preprocess_flat();

	// 背景统计
	back_stat_global();
	switch (param_->backStat.mode) {
	case FILTER_SPACE:
		back_stat_grid();
		break;
	case FILTER_FREQ_DOMAIN:
		break;
	default:
		break;
	}

	// 剔除坏像素
//	if (param_->preProc.badPixRemove) bad_pixels_remove();
	// 提取信号

	// 目标聚合

	// 计算目标特征

	// 处理特殊目标

	return true;
}

/*---------------------------------------------------------------------------*/
/* 功能: 预处理 */
void ADIReduce::load_preproc_zero() {
	if (param_->preProc.pathZero.empty())
		loadPreprocZero_ = 2;
	else
		loadPreprocZero_ = (fitsZero_.LoadImage(param_->preProc.pathZero.c_str()) == 0) ? 1 : 3;
}

void ADIReduce::load_preproc_dark() {
	if (param_->preProc.pathDark.empty())
		loadPreprocDark_ = 2;
	else
		loadPreprocDark_ = (fitsDark_.LoadImage(param_->preProc.pathDark.c_str()) == 0) ? 1 : 3;
}

void ADIReduce::load_preproc_flat() {
	if (param_->preProc.pathFlat.empty())
		loadPreprocFlat_ = 2;
	else {
		loadPreprocFlat_ = (fitsFlat_.LoadImage(param_->preProc.pathFlat.c_str()) == 0) ? 1 : 3;
		if (loadPreprocFlat_ == 1) {// 归一化平场
			unsigned wflat = fitsFlat_.wImg;
			unsigned hflat = fitsFlat_.hImg;
			unsigned pixels = wflat * hflat;
			float* flat = fitsFlat_.data;
			double mean(0.0), recip;
			for (unsigned i = 0; i < pixels; ++i) mean += flat[i];
			mean /= double(pixels);
			recip = 1.0 / mean; // 均值倒数
			for (unsigned i = 0; i < pixels; ++i, ++flat) *flat = float(*flat * recip);

			_gLog.Write("FLAT image scaled");
		}
	}
}

void ADIReduce::preprocess_zero() {
	// v1: 全帧图像, 不考虑ROI及BINNING
	unsigned wimg = fitsImg_.wImg;
	unsigned himg = fitsImg_.hImg;
	unsigned wzero = fitsZero_.wImg;
	unsigned hzero = fitsZero_.hImg;

	if (wimg != wzero || himg != hzero)
		_gLog.Write(LOG_WARN, "image dimension[%u, %u] does not match zero image[%u, %u]",
				wimg, himg, wzero, hzero);
	else {
		unsigned pixels = wimg * himg;
		float* img  = fitsImg_.data;
		float* zero = fitsZero_.data;
		for (unsigned i = 0; i < pixels; ++i, ++img, ++zero) *img -= *zero;
	}
}

void ADIReduce::preprocess_dark() {
	// v1: 全帧图像, 不考虑ROI及BINNING
	unsigned wimg = fitsImg_.wImg;
	unsigned himg = fitsImg_.hImg;
	unsigned wdark = fitsDark_.wImg;
	unsigned hdark = fitsDark_.hImg;

	if (wimg != wdark || himg != hdark)
		_gLog.Write(LOG_WARN, "image dimension[%u, %u] does not match dark image[%u, %u]",
				wimg, himg, wdark, hdark);
	else {
		unsigned pixels = wimg * himg;
		float* img  = fitsImg_.data;
		float* dark = fitsDark_.data;
		float t = fitsImg_.expdur;
		for (unsigned i = 0; i < pixels; ++i, ++img, ++dark) *img -= (*dark * t);
	}
}

void ADIReduce::preprocess_flat() {
	// v1: 全帧图像, 不考虑ROI及BINNING
	unsigned wimg = fitsImg_.wImg;
	unsigned himg = fitsImg_.hImg;
	unsigned wflat = fitsFlat_.wImg;
	unsigned hflat = fitsFlat_.hImg;

	if (wimg != wflat || himg != hflat)
		_gLog.Write(LOG_WARN, "image dimension[%u, %u] does not match flat image[%u, %u]",
				wimg, himg, wflat, hflat);
	else {
		unsigned pixels = wimg * himg;
		float* img  = fitsImg_.data;
		float* flat = fitsFlat_.data;

		for (unsigned i = 0; i < pixels; ++i, ++img, ++flat) *img /= *flat;
	}
}

/*---------------------------------------------------------------------------*/
/* 功能: 统计背景 */
void ADIReduce::back_stat_global() {
	unsigned wimg = fitsImg_.wImg;
	unsigned himg = fitsImg_.hImg;
	unsigned pixels = wimg * himg;
	float* img = fitsImg_.data;
	double mean(0.0), sig(0.0);

	for (unsigned i = 0; i < pixels; ++i, ++img) {
		mean += *img;
		sig  += *img * *img;
	}
	sig = (sig - mean * mean / pixels) / pixels;
	mean /= pixels;
	sig = sig > 0.0 ? sqrt(sig) : 0.0;
	frame_->bkMean  = mean;
	frame_->bkSigma = sig;

	_gLog.Write("background statistics. mean = %.1f, stdev = %.2f", mean, sig);
}

void ADIReduce::back_stat_grid() {
	// 备份原始数据, 并分配临时缓冲区
	buffPtr_->CopyData(fitsImg_.data, fitsImg_.wImg, fitsImg_.hImg);
}

/*---------------------------------------------------------------------------*/
/* 功能: 坏像素 */
void ADIReduce::bad_pixels_remove() {
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
	float pixv;

	// 遍历检测区
	for (y = y1, pos = y1 * w; y < y2; ++y, pos += w) {
		for (x = x1; x < x2; ++x) {
			if ((step = bad_pixel_neighbor(badMarked, w, x, y))) x += step;
			else if (bad_pixel_whether(bufSrc, w, x, y, pixv)) {
				badMarked[pos + x] = 1;
				bufDst[pos + x]    = pixv;
				++x;
			}
		}
	}

	// 释放临时存储区
	delete []badMarked;
}

bool ADIReduce::bad_pixel_whether(float* data, unsigned w, unsigned x, unsigned y, float& pixv) {
	unsigned pos = y * w + x;
	float val = data[pos], t;
	double mean(0.0), sig(0.0);
	int i, j, n(0);

	// 判定: 梯度
	for (j = -1, pos = pos - w; j <= 1; ++j, pos += w) {
		for (i = -1; i <= 1; ++i) {
			t = data[pos + i];
			if (val > t) {
				if (n < 0) return false;
				++n;
			}
			else if (val < t) {
				if (n > 0) return false;
				--n;
			}

			mean += t;
			sig  += t * t;
		}
	}
	// 判定: 阈值
	mean = (mean - val) * 0.125;
	sig -= val * val;
	sig  = (sig - 8.0 * mean * mean) / 7.0;
	if (sig <= 0.0) return false;
	sig = sqrt(sig);
	t = float((val - mean) / sig);
	if ((n > 0 && t < 10.0) || (n < 0 && t > -10.0)) return false;
	pixv = mean;

	_gLog.Write("%s: (%4u, %4u), sigma = %.1f, SNR = %.1f, %.0f ==> %.0f", n > 0 ? "Hot " : "Dark",
			x, y, sig, t, val, pixv);
	return true;
}

int ADIReduce::bad_pixel_neighbor(char* mask, unsigned w, unsigned x, unsigned y) {
	unsigned above = (y - 1) * w + x;
	if (mask[above])     return 1;  // 正上方
	if (mask[above + 1]) return 2;  // 右上方
	return 0;
}

/*---------------------------------------------------------------------------*/
