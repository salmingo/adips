/*!
 * @class AFindPV 关联识别视场内的运动目标目标
 * @version 0.1
 * @date 2021-04
 */

#include "AFindPV.h"

AFindPV::AFindPV(Parameter* param)
	: ADIProcess(param) {
	nameFunc_ = "finding motion objects";
}

AFindPV::~AFindPV() {

}

bool AFindPV::do_real_process() {
	return true;
}
