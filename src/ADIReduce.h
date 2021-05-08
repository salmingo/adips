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

	protected:
		unsigned pixOld;

	protected:
		bool resize(unsigned w, unsigned h) {
			unsigned pixNew = w * h;
			if (pixNew != pixOld && buff) {
				delete []buff;
				buff = NULL;
			}
			if (!buff) {
				buff   = new float[pixNew];
				pixOld = pixNew;
			}
			width  = w;
			height = h;
			return buff != NULL;
		}

	public:
		ImageBuffer() {
			width = height = 0;
			pixOld = 0;
			buff = NULL;
		}

		virtual ~ImageBuffer() {
			if (buff) delete []buff;
		}

		bool CopyData(float* data, unsigned w, unsigned h) {
			if (resize(w, h)) {
				memcpy(buff, data, w * h * sizeof(float));
				return true;
			}
			return false;
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
	void bad_pixels_remove();
	/*!
	 * @brief 检查是否坏像素, 并在确认是坏像素时计算其修正值
	 * @param data   原始数据
	 * @param w      图像帧宽度
	 * @param x      X坐标
	 * @param y      Y坐标
	 * @param pixv   修正值
	 * @return
	 * 坏像素判定结果
	 */
	bool bad_pixel_whether(float* data, unsigned w, unsigned x, unsigned y, float& pixv);
	/*!
	 * @brief 检查是否坏像素邻居
	 * @param mask  坏像素标记
	 * @param w      图像帧宽度
	 * @param x      X坐标
	 * @param y      Y坐标
	 * @return
	 * 判定结果
	 */
	int bad_pixel_neighbor(char* mask, unsigned w, unsigned x, unsigned y);
};

#endif /* ADIREDUCE_H_ */
