/**
 * @class BackStatSpace 在空域使用滤波算法统计图像背景和噪声
 * @version 0.1
 * @author Xiaomeng Lu
 */

#ifndef SRC_BACKSTATSPACE_H_
#define SRC_BACKSTATSPACE_H_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/shared_array.hpp>
#include "Parameter.hpp"
#include "ImageFrame.hpp"

class BackStatSpace {
public:
	BackStatSpace(Parameter* param);
	virtual ~BackStatSpace();

public:
	using Pointer = boost::shared_ptr<BackStatSpace>;
	using FltPtr  = boost::shared_array<float>;

protected:
	Parameter* param_;	/// 控制参数
	int wImg_;			/// 图像宽度
	int hImg_;			/// 图像高度
	const float* dataSrc_;	/// 原始图像数据
	FltPtr dataBuff1_;	/// 缓存区1
	FltPtr dataBuff2_;	/// 缓存区2
	FltPtr dataBuff3_;	/// 缓存区3

public:
	/*!
	 * @brief 创建实例指针
	 */
	static Pointer Create(Parameter* param) {
		return Pointer(new BackStatSpace(param));
	}
	/*!
	 * @brief 统计图像背景
	 * @param imgData  图像原始数据
	 * @param frame    图像帧
	 */
	void ProcImage(const float* imgData, ImgFrmPtr frame);
};
typedef BackStatSpace::Pointer BkSpacePtr;

#endif /* SRC_BACKSTATSPACE_H_ */
