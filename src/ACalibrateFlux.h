/*!
 * @class AFluxCalibrate 流量测光. 使用相对(较差)或绝对测光, 测量视场内目标的视星等
 * @version 0.1
 * @date 2021-04
 */

#ifndef ACALIBRATEFLUX_H_
#define ACALIBRATEFLUX_H_

#include "ImageFrame.hpp"

class AFluxCalibrate {
public:
	AFluxCalibrate();
	virtual ~AFluxCalibrate();

protected:
	bool working_;		/// 工作标志
	ImgFrmPtr frame_;	/// 待处理图像数据接口

public:
	/*!
	 * @brief 检测是否仍在执行处理流程处理流程
	 * @return
	 * 执行标志
	 */
	bool IsWorking();
};

#endif /* ACALIBRATEFLUX_H_ */
