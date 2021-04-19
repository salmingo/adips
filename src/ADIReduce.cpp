/*!
 * @class ADIReduce 天文数字图像处理. 从图像中提取目标信号并测量目标特征
 * @version 0.1
 * @date 2021-04
 */

#include "ADIReduce.h"

ADIReduce::ADIReduce() {
	working_ = false;
}

ADIReduce::~ADIReduce() {

}

bool ADIReduce::IsWorking() {
	return working_;
}
