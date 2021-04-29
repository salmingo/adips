/*!
 * @class APhotometry 流量测光. 使用相对(较差)或绝对测光, 测量视场内目标的视星等
 * @version 0.1
 * @date 2021-04
 */

#ifndef ACALIBRATEFLUX_H_
#define ACALIBRATEFLUX_H_

#include "ADIProcess.h"

class APhotometry : public ADIProcess {
public:
	APhotometry(Parameter* param);
	virtual ~APhotometry();

protected:
	/*!
	 * @brief 在多进程模式下执行真正的处理流程
	 */
	bool do_real_process();
};

#endif /* ACALIBRATEFLUX_H_ */
