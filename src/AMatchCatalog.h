/*!
 * @class AMatchCatalog 与天文星表匹配, 建立图像的WCS映射关系
 * @version 0.1
 * @date 2021-04
 */

#ifndef AMATCHCATALOG_H_
#define AMATCHCATALOG_H_

#include "ImageFrame.hpp"

class AMatchCatalog {
public:
	AMatchCatalog();
	virtual ~AMatchCatalog();

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

#endif /* AMATCHCATALOG_H_ */
