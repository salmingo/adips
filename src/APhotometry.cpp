/*!
 * @class APhotometry 流量测光. 使用相对(较差)或绝对测光, 测量视场内目标的视星等
 * @version 0.1
 * @date 2021-04
 */

#include "APhotometry.h"

APhotometry::APhotometry(Parameter* param)
	: ADIProcess(param) {
	nameFunc_ = "photometry";
}

APhotometry::~APhotometry() {

}

bool APhotometry::do_real_process() {
	return true;
}
