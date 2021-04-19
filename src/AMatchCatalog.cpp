/*!
 * @class AMatchCatalog 与天文星表匹配, 建立图像的WCS映射关系
 * @version 0.1
 * @date 2021-04
 */

#include "AMatchCatalog.h"

AMatchCatalog::AMatchCatalog() {
	working_ = false;
}

AMatchCatalog::~AMatchCatalog() {

}

bool AMatchCatalog::IsWorking() {
	return working_;
}
