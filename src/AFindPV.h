/*!
 * @class AFindPV 关联识别视场内的运动目标目标
 * @version 0.1
 * @date 2021-04
 */

#ifndef AASSOCIATEMOTION_H_
#define AASSOCIATEMOTION_H_

#include "ADIProcess.h"

class AFindPV : public ADIProcess {
public:
	AFindPV(Parameter* param);
	virtual ~AFindPV();

protected:
	/*!
	 * @brief 在多进程模式下执行真正的处理流程
	 */
	bool do_real_process();
};

#endif /* AASSOCIATEMOTION_H_ */
