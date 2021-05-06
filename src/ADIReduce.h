/*!
 * @class ADIReduce 天文数字图像处理. 从图像中提取目标信号并测量目标特征
 * @version 0.1
 * @date 2021-04
 */

#ifndef ADIREDUCE_H_
#define ADIREDUCE_H_

#include <string.h>
#include "ADIProcess.h"
#include "FITSHandlerImage.hpp"
#include "BackStatSpace.h"

class ADIReduce : public ADIProcess {
public:
	ADIReduce(Parameter* param);
	virtual ~ADIReduce();

protected:
	/*!
	 * @brief 图像帧大小数据缓存区
	 */
	struct ImageBuffer {
		unsigned width, height;
		float* buff;

	public:
		ImageBuffer() {
			width = height = 0;
			buff = NULL;
		}

		virtual ~ImageBuffer() {
			if (buff) delete []buff;
		}

		void Resize(unsigned w, unsigned h) {
			unsigned pixNew = w * h;
			unsigned pixOld = width * height;
			if (pixNew != pixOld && buff) {
				delete []buff;
				buff = NULL;
			}
			if (!buff) buff = new float[pixNew];
			width  = w;
			height = h;
		}

		bool CopyData(float* data, unsigned w, unsigned h) {
			Resize(w, h);
			if (!buff) return false;
			memcpy(buff, data, w * h * sizeof(float));
			return true;
		}
	};

protected:
	FITSHandlerImage fitsZero_;	/// 本底图像文件接口
	FITSHandlerImage fitsDark_;	/// 暗场图像文件接口
	FITSHandlerImage fitsFlat_;	/// 平场图像文件接口
	FITSHandlerImage fitsImg_;	/// FITS图像文件访问接口
	ImageBuffer imgBackup_;		/// 图像帧数据备份
	BkSpacePtr bkSpacePtr_;		/// 空域背景滤波接口
	/*!
	 * 预处理图像加载标志.
	 * 0: 未加载
	 * 1: 已加载
	 * 2: 未指定预处理文件
	 * 3: 不匹配
	 */
	int loadPreproc_;

protected:
	/*!
	 * @brief 在多进程模式下执行真正的处理流程
	 */
	bool do_real_process();
	/*!
	 * @brief 加载预处理图像帧数据
	 */
	void load_preproc_images();
	/*!
	 * @brief 检查预处理图像维度是否与图像帧相同
	 * @return
	 * 维度相同标志
	 * @note
	 * 支持图像帧为ROI模式
	 */
	bool precheck_dimension();
	/*!
	 * @brief 减本底
	 */
	void preprocess_zero();
	/*!
	 * @brief 减暗场
	 */
	void preprocess_dark();
	/*!
	 * @brief 除平场
	 */
	void preprocess_flat();

protected:
	/* 接口: 坏像素 */
	/*!
	 * @brief 移除坏像素
	 */
	void remove_bad_pixels();
	bool check_bad_pixel(float* data, unsigned w, unsigned h, unsigned x, unsigned y);
	int  check_bad_pixel_neighbor(char* mask, unsigned w, unsigned h, unsigned x, unsigned y);
	float correct_bad_pixel(float* data, unsigned w, unsigned h, unsigned x, unsigned y);
};

#endif /* ADIREDUCE_H_ */
