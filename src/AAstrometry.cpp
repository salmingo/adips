/*!
 * @class AAstrometry 与天文星表匹配, 建立图像的WCS映射关系
 * @version 0.1
 * @date 2021-04
 */

#include "AAstrometry.h"

AAstrometry::AAstrometry(Parameter* param)
	: ADIProcess(param) {
	nameFunc_ = "astrometry";
}

AAstrometry::~AAstrometry() {

}

bool AAstrometry::do_real_process() {

	frame_->succAstro = true;
	return true;
}
