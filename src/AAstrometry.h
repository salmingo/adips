/*!
 * @class AAstrometry 与天文星表匹配, 建立图像的WCS映射关系
 * @version 0.1
 * @date 2021-04
 */

#ifndef AMATCHCATALOG_H_
#define AMATCHCATALOG_H_

#include "ADIProcess.h"

class AAstrometry : public ADIProcess {
public:
	AAstrometry(Parameter* param);
	virtual ~AAstrometry();

protected:
	/*!
	 * @brief 在多进程模式下执行真正的处理流程
	 */
	bool do_real_process();
};

#endif /* AMATCHCATALOG_H_ */
