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

	/*!
	 * @struct MemoryBuffer 用于图像处理的内存缓冲区管理接口
	 */
	struct MemoryBuffer {
		unsigned wImg, hImg;	/// 图像帧像素数
		unsigned nbkx, nbky;	/// XY方向背景网格数量
		float* backup;		/// 原始数据备份
		float* bkmean;		/// 背景网格均值
		float* bksig;		/// 背景网格噪声

	protected:
		unsigned bkw, bkh;	/// 背景网格宽度和高度

	public:
		MemoryBuffer(unsigned bkGridW, unsigned bkGridH) {
			bkw = bkGridW;
			bkh = bkGridH;
			wImg = hImg = 0;
			nbkx = nbky = 0;
			backup = NULL;
			bkmean = bksig = NULL;
		}

		virtual ~MemoryBuffer() {
			if (backup) delete []backup;
			if (bkmean) delete []bkmean;
			if (bksig)  delete []bksig;
		}

		bool CopyData(float* data, unsigned wNew, unsigned hNew) {
			if (!resize(wNew, hNew)) return false;
			memcpy(backup, data, wNew * hNew * sizeof(float));
			return true;
		}

	protected:
		bool resize(unsigned wNew, unsigned hNew) {
			unsigned pixOld = wImg * hImg;
			unsigned pixNew = wNew * hNew;
			// 检查并重新分配备份区
			if (pixOld != pixNew && backup) {
				delete []backup;
				backup = NULL;
			}
			if (!backup) backup = new float[pixNew];
			wImg = wNew;
			hImg = hNew;
			// 检查并重新分配网格区
			pixOld = nbkx * nbky;
			nbkx = (wNew - 1) / bkw + 1;
			nbky = (hNew - 1) / bkh + 1;
			pixNew = nbkx * nbky;
			if (pixOld != pixNew) {
				if (bkmean) delete[] bkmean;
				if (bksig)  delete[] bksig;
				bkmean = bksig = NULL;
			}
			if (!bkmean) bkmean = new float[pixNew];
			if (!bksig)  bksig  = new float[pixNew];

			return (backup != NULL && bkmean != NULL && bksig != NULL);
		}
	};
	using MembuffPtr = boost::shared_ptr<MemoryBuffer>;

protected:
	/*!
	 * 预处理图像加载标志.
	 * 0: 未加载
	 * 1: 已加载
	 * 2: 未指定预处理文件
	 * 3: 数据加载失败
	 */
	int loadPreprocZero_;
	int loadPreprocDark_;
	int loadPreprocFlat_;
	FITSHandlerImage fitsZero_;	/// 本底图像文件接口
	FITSHandlerImage fitsDark_;	/// 暗场图像文件接口
	FITSHandlerImage fitsFlat_;	/// 平场图像文件接口
	FITSHandlerImage fitsImg_;	/// FITS图像文件访问接口
	MembuffPtr buffPtr_;		/// 数据处理内存缓冲区
	ImageBuffer imgBackup_;		/// 图像帧数据备份

protected:
	/* 功能: 数据处理流程 */
	/*!
	 * @brief 在多进程模式下执行真正的处理流程
	 */
	bool do_real_process();

protected:
	/* 功能: 预处理 */
	/*!
	 * @brief 加载预处理图像帧数据: 合并后本底
	 */
	void load_preproc_zero();
	/*!
	 * @brief 加载预处理图像帧数据: 合并后暗场
	 */
	void load_preproc_dark();
	/*!
	 * @brief 加载预处理图像帧数据: 合并后平场
	 */
	void load_preproc_flat();
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
	/* 功能: 背景统计 */
	/*!
	 * @brief 统计全帧图像背景, 用于调节图像对比度
	 */
	void back_stat_global();
	/*!
	 * @brief 在空域完成背景统计...
	 */
	void back_stat_grid();

protected:
	/* 功能: 坏像素 */
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
