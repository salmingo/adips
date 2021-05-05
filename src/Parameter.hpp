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

enum {
	FILTER_SPACE = 1,	/// 空域滤波
	FILTER_FREQ_DOMAIN	/// 频域滤波
};

struct Parameter {
	/* 功能 */
	struct {
		bool use_astrometry;	/// 天文定位
		bool use_photometry;	/// 测光
		bool use_motion;		/// 运动关联
	} funcs;

	/* 图像处理 */
	// 预处理
	struct {
		string path_work;	/// 工作路径. 处理结果存储在该目录下
		string path_zero;	/// 合并后本底路径
		string path_dark;	/// 合并后暗场路径
		string path_flat;	/// 合并后平场路径
		bool remove_badpix;	/// 剔除坏像素
	} preProc;

	// 背景算法
	struct {
		bool bk_global;		/// 使用全局背景
		int bk_mode;		/// 背景滤波算法. 1: 空域滤波; 2: 频域滤波
		int bk_grid_width;	/// 背景网格宽度
		int bk_grid_height;	/// 背景网格高度
		int bk_filter_x;	/// 背景X方向滤波宽度
		int bk_filter_y;	/// 背景Y方向滤波高度
	} backStat;

	// 信号提取参数
	struct {
		bool sigUseFilter;		/// 检测信号前应用滤波算法
		float sigSNR;			/// 最小信噪比
	} sigExtract;

	// 信号测量参数
	struct {
		int blobPixMin;			/// 构成目标的最小像素数
		int blobPixMax;			/// 构成目标的最大像素数. 0: 无限制
	} blobMeasure;

	// 目标输出参数
	struct {
		bool outputInterResult;	/// 输出中间结果, 包括滤波后背景、噪声等
		bool outputFinalResult;	/// 输出处理结果, 包括所有被识别目标
		bool outputAloneWCS;	/// 输出单独的WCS文件. 否则写入原始FITS头
	} output;

	/* CMOS相机时间修正参数 */
	struct {
		bool clockCorrect;		/// 使用时间修正
		int msPreClean;			/// 收到曝光指令后的芯片清空时间, 量纲: 毫秒
		int msLinesShift;		/// 图像行转移时间, 量纲： 毫秒
	} cmosCorrect;

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
		node1.add("Astrometry.<xmlattr>.Enable",  false);
		node1.add("Photometry.<xmlattr>.Enable",  false);
		node1.add("Motion.<xmlattr>.Enable",      false);

		ptree& node2 = nodes.add("PreProcess", "");
		node2.add("Work.<xmlattr>.Dir",  "");
		node2.add("ZERO.<xmlattr>.Path", "");
		node2.add("DARK.<xmlattr>.Path", "");
		node2.add("FLAT.<xmlattr>.Path", "");
		node2.add("RemoveBadPixel.<xmlattr>.Enable", true);

		ptree& node3 = nodes.add("BackGround",   "");
		node3.add("Global.<xmlattr>.Enable",     false);
		node3.add("Filter.<xmlattr>.Mode",       1);
		node3.add("Grid.<xmlattr>.Width",        32);
		node3.add("Grid.<xmlattr>.Height",       32);
		node3.add("Filter.<xmlattr>.Horizontal", 3);
		node3.add("Filter.<xmlattr>.Vertical",   3);

		ptree& node4 = nodes.add("ResolveSignal",    "");
		node4.add("Filter.<xmlattr>.Enable",         false);
		node4.add("Filter.<xmlattr>.FilePath",       "/usr/local/etc/adips/default.conv");
		node4.add("SNR.<xmlattr>.Minimum",           1.5);

		ptree& node5 = nodes.add("BlobMesurement", "");
		node5.add("PixelNumber.<xmlattr>.Minimum", 1);
		node5.add("PixelNumber.<xmlattr>.Maximum", 4);

		ptree& node6 = nodes.add("Output", "");
		node6.add("Result.<xmlattr>.Final",        true);
		node6.add("Result.<xmlattr>.Intermediate", true);
		node6.add("WCS.<xmlattr>.Alone",           true);

		ptree& node7 = nodes.add("ClockCorrect-for-CMOS",  "");
		node7.add("<xmlattr>.Enable",     false);
		node7.add("<xmlattr>.PreClean",   0);
		node7.add("<xmlattr>.LinesShift", 0);

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
		using namespace boost::property_tree;
		using namespace boost::property_tree::xml_parser;

		try {
			string str;
			ptree nodes;

			read_xml(filepath, nodes, trim_whitespace);
			BOOST_FOREACH(ptree::value_type const &child, nodes.get_child("")) {
				if (boost::iequals(child.first, "Function")) {
					useAstrometry = child.second.get("Astrometry.<xmlattr>.Enable",  false);
					usePhotometry = child.second.get("Photometry.<xmlattr>.Enable",  false);
					useMotion     = child.second.get("Motion.<xmlattr>.Enable",      false);
				}
				else if (boost::iequals(child.first, "PreProcess")) {
					pathWork = child.second.get("Work.<xmlattr>.Dir",  "");
					pathZero = child.second.get("ZERO.<xmlattr>.Path", "");
					pathDark = child.second.get("DARK.<xmlattr>.Path", "");
					pathFlat = child.second.get("FLAT.<xmlattr>.Path", "");
					preRemoveBadpix = child.second.get("RemoveBadPixel.<xmlattr>.Enable", false);
				}
				else if (boost::iequals(child.first, "BackGround")) {
					backGlobal     = child.second.get("Global.<xmlattr>.Enable",     false);
					backMode       = child.second.get("Filter.<xmlattr>.Mode",       FILTER_SPACE);
					backGridHor    = child.second.get("Grid.<xmlattr>.Horizontal",   32);
					backGridVer    = child.second.get("Grid.<xmlattr>.Vertical",     32);
					backFilterHor  = child.second.get("Filter.<xmlattr>.Horizontal", 3);
					backFilterVer  = child.second.get("Filter.<xmlattr>.Vertical",   3);
				}
				else if (boost::iequals(child.first, "ResolveSignal")) {
					sigUseFilter    = child.second.get("Filter.<xmlattr>.Enable",         false);
					sigPathFilter   = child.second.get("Filter.<xmlattr>.FilePath",       "");
					sigSNR          = child.second.get("SNR.<xmlattr>.Minimum",           1.5);
				}
				else if (boost::iequals(child.first, "BlobMesurement")) {
					blobPixMin = child.second.get("PixelNumber.<xmlattr>.Minimum",  1);
					blobPixMax = child.second.get("PixelNumber.<xmlattr>.Maximum",  0);

					if (blobPixMin <= 0) blobPixMin = 1;
					if (blobPixMax <= 0) blobPixMax = 0;
				}
				else if (boost::iequals(child.first, "Output")) {
					outputFinalResult = child.second.get("Result.<xmlattr>.Final",         false);
					outputInterResult = child.second.get("Result.<xmlattr>.Intermediate",  false);
					outputAloneWCS    = child.second.get("WCS.<xmlattr>.Alone",            false);
				}
				else if (boost::iequals(child.first, "Clock-Correct-For-CMOS")) {
					clockCorrect = child.second.get("<xmlattr>.Enable",     false);
					msPreClean   = child.second.get("<xmlattr>.PreClean",   0);
					msLinesShift = child.second.get("<xmlattr>.LinesShift", 0);
				}
			}

			return true;
		}
		catch(xml_parser_error &ex) {
			return false;
		}
	}
};

#endif
