/*!
 * @class ADIReduce 天文数字图像处理. 从图像中提取目标信号并测量目标特征
 * @version 0.1
 * @date 2021-04
 */

#ifndef ADIREDUCE_H_
#define ADIREDUCE_H_

#include "ADIProcess.h"
#include "FITSHandlerImage.hpp"

class ADIReduce : public ADIProcess {
public:
	ADIReduce(Parameter* param);
	virtual ~ADIReduce();

protected:
	int maxThread_;		/// 线程并发数. 硬件数量的一半
	FITSHandlerImage hFITS_;	/// FITS图像文件访问接口

protected:
	/*!
	 * @brief 在多进程模式下执行真正的处理流程
	 */
	bool do_real_process();
};

#endif /* ADIREDUCE_H_ */
