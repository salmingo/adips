/**
 * @file FilterMean.hpp 均值滤波器接口
 * @version 1.0
 * @note
 * 均值滤波用途:
 * -
 */

#ifndef SRC_FILTERMEAN_HPP_
#define SRC_FILTERMEAN_HPP_

#include <string.h>

namespace Filter {
//////////////////////////////////////////////////////////////////////////////
/*!
 * @struct FilterMeanKernel 声明均值滤波核
 */
struct FilterMeanKernel {
protected:
	unsigned wk, hk;	/// 滤波窗口

public:
	/*!
	 * @brief 构造函数
	 * @param w  滤波窗口宽度
	 * @param h  滤波窗口高度
	 */
	FilterMeanKernel(unsigned w, unsigned h) {
		if (!(w & 1)) ++w;
		if (!(h & 1)) ++h;
		if (w < 3) w = 3;
		if (h < 3) h = 3;
		wk = w;
		hk = h;
	}

public:
	template<class T> void Convolver(T* data, unsigned w, unsigned h) {
		unsigned xhalf  = wk / 2;
		unsigned right  = w - xhalf;
		unsigned yhalf  = hk / 2;
		unsigned bottom = h - yhalf;
		unsigned x, y, x1, y1, x2, y2;
		double sum;
		double scale = 1.0 / (wk * hk);
		double *cols = new double[w];
		T *back = new T[w * h];
		T *bkptr, *dptr, *cptr;

		memcpy (back, data, sizeof(T) * w * h);
		memset (cols, 0, sizeof(double) * w);

		for (y = 0, dptr = data; y < hk; ++y, dptr += w) {
			for (x = 0, cptr = cols; x < wk; ++x, ++cptr)
				*cptr += dptr[x];
		}

		for (y1 = yhalf; y1 < bottom; ++y1) {
			bkptr = back + (y1 - yhalf) * w;
			for (x1 = xhalf; x1 < right; ++x1) {

			}
		}

		delete []back;
		delete []cols;
	}
};

//////////////////////////////////////////////////////////////////////////////
};

#endif /* SRC_FILTERMEAN_HPP_ */
