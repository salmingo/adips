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

struct ParamFunction {
	bool useAstrometry;	/// 天文定位
	bool usePhotometry;	/// 测光
	bool useMotion;		/// 运动关联
};

struct ParamPreProcess {
	string pathWork;	/// 工作路径. 处理结果存储在该目录下
	string pathZero;	/// 合并后本底路径
	string pathDark;	/// 合并后暗场路径
	string pathFlat;	/// 合并后平场路径
	bool badPixRemove;	/// 剔除坏像素
};

// 背景算法
struct ParamBackground {
	bool useGlobal;		/// 使用全局背景
	int mode;			/// 背景滤波算法. 1: 空域滤波; 2: 频域滤波
	unsigned gridWidth;	/// 背景网格宽度
	unsigned gridHeight;/// 背景网格高度
	unsigned filterX;	/// 背景X方向滤波宽度
	unsigned filterY;	/// 背景Y方向滤波高度
};

struct ParamExtractSignal {
	int modeFilter;		/// 检测信号前应用滤波算法
	float sigMin;		/// 最小信噪比
};

// 目标测量参数
struct ParamMeasureBlob {
	unsigned pixMin;	/// 构成目标的最小像素数
	unsigned pixMax;	/// 构成目标的最大像素数. 0: 无限制
};

struct ParamOutput {
	bool rsltInter;	/// 输出中间结果, 包括滤波后背景、噪声等
	bool rsltFinal;	/// 输出处理结果, 包括所有被识别目标
	bool wcsAlone;	/// 输出单独的WCS文件. 否则写入原始FITS头
};

struct ParamCorrectClock {
	bool correct;			/// 使用时间修正
	unsigned msPreClean;	/// 收到曝光指令后的芯片清空时间, 量纲: 毫秒
	unsigned msLinesShift;	/// 图像行转移时间, 量纲： 毫秒
};

struct Parameter {
	/* 功能 */
	ParamFunction funcs;

	/* 图像处理 */
	ParamPreProcess preProc;		// 预处理
	ParamBackground backStat;		// 统计背景
	ParamExtractSignal sigExtract;	// 信号提取参数
	ParamMeasureBlob blobMeasure;	// 测量目标
	ParamOutput output;				// 目标输出参数

	/* CMOS相机时间修正参数 */
	ParamCorrectClock clockCorrect;	// 时间修正

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
		node3.add("Filter.<xmlattr>.X",          3);
		node3.add("Filter.<xmlattr>.Y",          3);

		ptree& node4 = nodes.add("ResolveSignal",    "");
		node4.add("Filter.<xmlattr>.Mode",     0);
		node4.add("SNR.<xmlattr>.Minimum",     1.5);

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
					funcs.useAstrometry = child.second.get("Astrometry.<xmlattr>.Enable",  false);
					funcs.usePhotometry = child.second.get("Photometry.<xmlattr>.Enable",  false);
					funcs.useMotion     = child.second.get("Motion.<xmlattr>.Enable",      false);
				}
				else if (boost::iequals(child.first, "PreProcess")) {
					preProc.pathWork = child.second.get("Work.<xmlattr>.Dir",  "");
					preProc.pathZero = child.second.get("ZERO.<xmlattr>.Path", "");
					preProc.pathDark = child.second.get("DARK.<xmlattr>.Path", "");
					preProc.pathFlat = child.second.get("FLAT.<xmlattr>.Path", "");
					preProc.badPixRemove = child.second.get("RemoveBadPixel.<xmlattr>.Enable", false);
				}
				else if (boost::iequals(child.first, "BackGround")) {
					backStat.useGlobal   = child.second.get("Global.<xmlattr>.Enable",     false);
					backStat.mode        = child.second.get("Filter.<xmlattr>.Mode",       1);
					backStat.gridWidth   = child.second.get("Grid.<xmlattr>.Width",        32);
					backStat.gridHeight  = child.second.get("Grid.<xmlattr>.Height",       32);
					backStat.filterX     = child.second.get("Filter.<xmlattr>.X",          3);
					backStat.filterY     = child.second.get("Filter.<xmlattr>.Y",          3);

					if (backStat.gridWidth < 16)   backStat.gridWidth = 16;
					if (backStat.gridHeight < 16)  backStat.gridHeight = 16;
					if (backStat.filterX < 1)      backStat.filterX = 1;
					if (backStat.filterY < 1)      backStat.filterY = 1;
				}
				else if (boost::iequals(child.first, "ResolveSignal")) {
					sigExtract.modeFilter = child.second.get("Filter.<xmlattr>.Mode",     0);
					sigExtract.sigMin     = child.second.get("SNR.<xmlattr>.Minimum",     1.5);
					if (sigExtract.sigMin < 1.0) sigExtract.sigMin = 1.0;
				}
				else if (boost::iequals(child.first, "BlobMesurement")) {
					blobMeasure.pixMin = child.second.get("PixelNumber.<xmlattr>.Minimum",  1);
					blobMeasure.pixMax = child.second.get("PixelNumber.<xmlattr>.Maximum",  0);

					if (blobMeasure.pixMin == 0) blobMeasure.pixMin = 1;
				}
				else if (boost::iequals(child.first, "Output")) {
					output.rsltFinal = child.second.get("Result.<xmlattr>.Final",         false);
					output.rsltInter = child.second.get("Result.<xmlattr>.Intermediate",  false);
					output.wcsAlone  = child.second.get("WCS.<xmlattr>.Alone",            false);
				}
				else if (boost::iequals(child.first, "Clock-Correct-For-CMOS")) {
					clockCorrect.correct = child.second.get("<xmlattr>.Enable",     false);
					clockCorrect.msPreClean   = child.second.get("<xmlattr>.PreClean",   0);
					clockCorrect.msLinesShift = child.second.get("<xmlattr>.LinesShift", 0);
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
