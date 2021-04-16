/*!
 * @file Parameter.hpp 使用XML格式维护关联配置参数
 * @version 0.1
 * @date 2021-04-16
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

using std::string;

struct Parameter {
	/* 启用功能 */
	bool useAstrometry;	/// 天文定位
	bool usePhotometry;	/// 测光
	bool useMotionAss;	/// 运动关联

	/* 图像处理 */
	// 预处理
	string pathWork;	/// 工作路径. 处理结果存储在该目录下
	string pathZero;	/// 合并后本底路径
	string pathDark;	/// 合并后暗场路径
	string pathFlat;	/// 合并后平场路径

	// 背景算法

	// 信号提取参数

	// 信号聚合参数

	// 目标输出参数

	/* CMOS相机时间修正参数 */
	int msPreClean;		/// 收到曝光指令后的芯片清空时间, 量纲: 毫秒
	int msLinesShift;	/// 图像行转移时间, 量纲： 毫秒

public:
	/*!
	 * @brief 初始化配置参数文件
	 * @param filepath 文件路径
	 */
	void Init(const string &filepath) {
		using namespace boost::property_tree;
		using namespace boost::property_tree::xml_parser;
		using namespace boost::posix_time;

		ptree nodes;
		nodes.add("version", "0.1");
		nodes.add("date", to_iso_extended_string(second_clock::universal_time()));

		ptree& node1 = nodes.add("Function", "");
		node1.add("Astrometry.<xmlattr>.Enable",        false);
		node1.add("Photometry.<xmlattr>.Enable",        false);
		node1.add("MotionAssociation.<xmlattr>.Enable", false);

		ptree& node2 = nodes.add("ClockCorrect-for-CMOS",  "");
		node2.add("<xmlattr>.PreClean",   0);
		node2.add("<xmlattr>.LinesShift", 0);

		ptree& node3 = nodes.add("PreProcess", "");
		node3.add("Work.<xmlattr>.Dir",  "");
		node3.add("ZERO.<xmlattr>.Path", "");
		node3.add("DARK.<xmlattr>.Path", "");
		node3.add("FLAT.<xmlattr>.Path", "");

		xml_writer_settings<string> settings(' ', 4);
		write_xml(filepath, nodes, std::locale(), settings);
	}

	/*!
	 * @brief 加载配置参数文件
	 * @param filepath 文件路径
	 * @return
	 * 文件加载结果
	 */
	bool Load(const string &filepath) {
		try {
			using namespace boost::property_tree;
			using namespace boost::property_tree::xml_parser;

			string str;
			ptree nodes;

			read_xml(filepath, nodes, trim_whitespace);
			BOOST_FOREACH(ptree::value_type const &child, nodes.get_child("")) {
				if (boost::iequals(child.first, "Function")) {
					useAstrometry = child.second.get("Astrometry.<xmlattr>.Enable",        false);
					usePhotometry = child.second.get("Photometry.<xmlattr>.Enable",        false);
					useMotionAss  = child.second.get("MotionAssociation.<xmlattr>.Enable", false);
				}
				else if (boost::iequals(child.first, "Clock-Correct-For-CMOS")) {
					msPreClean   = child.second.get("<xmlattr>.PreClean",   0);
					msLinesShift = child.second.get("<xmlattr>.LinesShift", 0);
				}
				else if (boost::iequals(child.first, "PreProcess")) {
					pathWork = child.second.get("Work.<xmlattr>.Dir",  "");
					pathZero = child.second.get("ZERO.<xmlattr>.Path", "");
					pathDark = child.second.get("DARK.<xmlattr>.Path", "");
					pathFlat = child.second.get("FLAT.<xmlattr>.Path", "");
				}
			}

			return true;
		}
		catch(boost::property_tree::xml_parser::xml_parser_error &ex) {
			Init(filepath);
			return false;
		}
	}
};

#endif
