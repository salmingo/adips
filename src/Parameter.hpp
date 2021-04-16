/*!
 * @file Parameter.hpp 使用XML格式维护关联配置参数
 * @version 0.1
 * @date 2021-04-16
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <string>
#include <boost/property_tree/ptree.hpp>

using std::string;

struct Parameter {
	/* 启用功能 */
	bool useAstrometry;	/// 天文定位
	bool usePhotometry;	/// 测光
	bool useMotionAss;	/// 运动关联

	/* 图像处理 */
	// 背景算法

	// 信号提取参数

	// 信号聚合参数

	// 目标输出参数


public:
	/*!
	 * @brief 初始化配置参数文件
	 * @param filepath 文件路径
	 */
	void Init(const string &filepath) {

	}
};
