/*!
 * @class AFluxCalibrate 流量测光. 使用相对(较差)或绝对测光, 测量视场内目标的视星等
 * @version 0.1
 * @date 2021-04
 */

#include "ACalibrateFlux.h"

AFluxCalibrate::AFluxCalibrate() {
	working_ = false;
}

AFluxCalibrate::~AFluxCalibrate() {

}

bool AFluxCalibrate::IsWorking() {
	return working_;
}
