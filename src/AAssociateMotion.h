/*!
 * @class AMotioAssociate 关联识别视场内的运动目标目标
 * @version 0.1
 * @date 2021-04
 */

#ifndef AASSOCIATEMOTION_H_
#define AASSOCIATEMOTION_H_

#include "ImageFrame.hpp"

class AMotioAssociate {
public:
	AMotioAssociate();
	virtual ~AMotioAssociate();

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

#endif /* AASSOCIATEMOTION_H_ */
