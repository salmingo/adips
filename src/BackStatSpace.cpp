/**
 * @class BackStatFixed 使用固定频率统计图像背景和噪声
 * @version 0.1
 * @author Xiaomeng Lu
 */

#include "BackStatSpace.h"

BackStatSpace::BackStatSpace(Parameter* param)
	: param_(param) {
	wImg_ = hImg_ = 0;
	dataSrc_ = NULL;
}

BackStatSpace::~BackStatSpace() {

}

void BackStatSpace::ProcImage(const float* imgData, ImgFrmPtr frame) {
	int pixOld = wImg_ * hImg_;
	int pixNew = frame->wImg * frame->hImg;
	if (pixOld != pixNew) dataBuff1_.reset(new float[pixNew]);
	wImg_ = frame->wImg;
	hImg_ = frame->hImg;
	dataSrc_ = imgData;

}
