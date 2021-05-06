/*!
 * @class FITSHandlerImage  FITS图像文件访问接口
 * @version 0.2
 * @date 2021-04-16
 * @note
 * - 打开FITS文件
 * - 读取关键头信息: 曝光时间, 曝光起始时间
 *   曝光时间特征字: EXPTIME/EXPOSURE//EXPDUR
 *   曝光起始时间特征字: DATE-OBS/TIME-OBS
 * - 以float类型将数据读入内存
 */

#ifndef FITSHANDLER_IMAGE_H_
#define FITSHANDLER_IMAGE_H_

#include <longnam.h>
#include <fitsio.h>
#include <string>

struct FITSHandlerImage {
public:
	/* 成员变量 */
	float* data;				/// 图像数据存储区
	unsigned wImg, hImg;		/// 图像大小
	unsigned xStart, yStart;	/// ROI区起始地址: 1
	unsigned xBin, yBin;		/// ROI区合并因子
	std::string dateobs;		/// 曝光起始时间, 格式: CCYY-MM-DDThh:mm:ss<.sss<sss>>, UTC
	double expdur;				/// 曝光时间, 量纲: 秒

public:
	/* 构造与析构函数 */
	FITSHandlerImage() {
		data = NULL;
		wImg = hImg = 0;
		xStart = yStart = 1;
		xBin = yBin = 1;
		expdur = 0.0;
	}

	virtual ~FITSHandlerImage() {
		if (data) delete []data;
	}

public:
	/* 接口 */
	/*!
	 * @brief 加载FITS图像文件
	 * @param filepath 文件路径
	 * @return
	 * 文件加载结果.
	 * 0: 加载成功
	 * 1: 文件格式错误
	 * 2: 文件头缺少关键信息
	 * 3: 数据读入错误
	 */
	int LoadImage(const char* filepath) {
		fitsfile *hFits;
		int state(0);
		unsigned w, h;
		char obsdate[30], obstime[30], tmfull[40];
		bool datefull;

		// 尝试打开文件
		fits_open_image(&hFits, filepath, 0, &state);
		if (state) return 1;
		// 读取关键文件头信息
		fits_read_key(hFits, TUINT, "NAXIS1", &w, NULL, &state);
		fits_read_key(hFits, TUINT, "NAXIS2", &h, NULL, &state);
		fits_read_key(hFits, TSTRING, "DATE-OBS", obsdate,  NULL, &state);
		if (!(datefull = NULL != strstr(obsdate, "T")))
			fits_read_key(hFits, TSTRING, "TIME-OBS", obstime,  NULL, &state);
		if (state) {
			close_file(hFits);
			return 2;
		}
		if (!datefull) sprintf(tmfull, "%sT%s", obsdate, obstime);
		dateobs = tmfull;
		// 尝试不同关键字表征的曝光时间
		fits_read_key(hFits, TDOUBLE, "EXPOSURE",  &expdur, NULL, &state);
		if (state) {
			state = 0;
			fits_read_key(hFits, TDOUBLE, "EXPTIME",  &expdur, NULL, &state);
		}
		if (state) {
			state = 0;
			fits_read_key(hFits, TDOUBLE, "EXPDUR",  &expdur, NULL, &state);
		}
		dateobs = tmfull;
		// 数据读入内存
		alloc_buff(w, h);
		fits_read_img(hFits, TFLOAT, 1, w * h, NULL, data, NULL, &state);
		close_file(hFits);

		return state ? 3 : 0;
	}

	bool LookImage(const char* filepath) {
		fitsfile *hFits;
		int state(0);
		unsigned w, h;

		// 尝试打开文件
		fits_open_image(&hFits, filepath, 0, &state);
		if (state) return false;
		// 读取关键文件头信息
		fits_read_key(hFits, TUINT, "NAXIS1", &w, NULL, &state);
		fits_read_key(hFits, TUINT, "NAXIS2", &h, NULL, &state);
		alloc_buff(w, 1);
		hImg = h;
		close_file(hFits);

		return true;
	}

	/*!
	 * @brief 获取全帧图像数据
	 * @param w  图像宽度
	 * @param h  图像高度
	 * @return
	 * 数据存储区地址
	 */
	float* GetImage(unsigned &w, unsigned &h) {
		w = wImg;
		h = hImg;
		return data;
	}

	/*!
	 * @brief 读取第line行数据
	 * @param line 行编号, 起始地址: 0
	 * @return
	 * 数据存储区地址
	 */
	float *GetLine(const char* filepath, unsigned line) {
		fitsfile *hFits;
		int state(0);
		fits_open_image(&hFits, filepath, 0, &state);
		fits_read_img(hFits, TFLOAT, 1 + line * wImg, wImg, NULL, data, NULL, &state);
		close_file(hFits);
		return data;
	}

	/*!
	 * @brief 查看曝光时间
	 * @param t  曝光时间长度
	 * @return
	 * 曝光起始时间
	 */
	const char* GetExptime(double& t) {
		t = expdur;
		return dateobs.c_str();
	}

protected:
	/* 功能 */
	void alloc_buff(unsigned w, unsigned h) {
		unsigned pixNew = w * h;
		unsigned pixOld = wImg * hImg;
		if (pixNew != pixOld && data != NULL) {
			delete []data;
			data = NULL;
		}
		if (data == NULL) data = new float[pixNew];
		wImg = w;
		hImg = h;
	}

	void close_file(fitsfile *h) {
		int state(0);
		fits_close_file(h, &state);
	}
};

#endif
