/*!
 * @class AMotioAssociate 关联识别视场内的运动目标目标
 * @version 0.1
 * @date 2021-04
 */

#include "AAssociateMotion.h"

AMotioAssociate::AMotioAssociate() {
	working_ = false;
}

AMotioAssociate::~AMotioAssociate() {

}

bool AMotioAssociate::IsWorking() {
	return working_;
}
