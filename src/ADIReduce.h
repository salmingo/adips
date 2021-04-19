/*!
 * @class ADIReduce 天文数字图像处理. 从图像中提取目标信号并测量目标特征
 * @version 0.1
 * @date 2021-04
 */

#ifndef ADIREDUCE_H_
#define ADIREDUCE_H_

#include "ImageFrame.hpp"

class ADIReduce {
public:
	ADIReduce();
	virtual ~ADIReduce();

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

#endif /* ADIREDUCE_H_ */
