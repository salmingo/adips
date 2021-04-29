/*!
 * @struct ImageFrame 定义图像数据及其处理结果的数据结构
 * @version 0.1
 * @date 2021-04-16
 */

#ifndef IMAGEFRAME_H_
#define IMAGEFRAME_H_

#include <string>
#include <vector>
#include <deque>
#include <strings.h>
#include <boost/smart_ptr/shared_ptr.hpp>

/*!
 * @brief 定义2维坐标, 坐标以实数表示
 */
struct point_2f {
	double x, y;
};

/*!
 * @brief 定义3维坐标, 坐标以实数表示
 */
struct point_3f {
	double x, y, z;
};

/*!
 * @struct CelestialBody 定义天体物理特征
 */
struct CelestialBody {
	point_2f ptBary;	/// 质心
	point_2f ptCenter;	/// 几何中心
	point_3f ptPeak;	/// 峰值位置及亮度
	point_2f ptEquator;	/// 质心对应的赤道坐标, 量纲: 角度
	double ellipcity;	/// 椭率
	double a, b;		/// 半长轴和半短轴
	double tilt;		/// 倾角. 长轴和X轴正向的夹角, 量纲: 角度
	double area;		/// 面积
	double back;		/// 质心背景亮度
	double noise;		/// 质心噪声
	double flux;		/// 积分流量
	double snr;			/// 信噪比
	int type;			/// 匹配类型. 0: 未匹配; 1: 恒星/星系/星团

public:
	CelestialBody() {
		bzero(this, sizeof(CelestialBody));
	}
};
typedef std::vector<CelestialBody> CeleBodyVec;

struct ImageFrame {
	/* 文件原始信息 */
	std::string filepath;	/// 全路径名
	std::string pathdir;	/// 目录名
	std::string filename;	/// 文件名
	std::string filetit;	/// 文件名(不含扩展名)
	std::string dateobs;	/// 曝光起始时间, 格式: CCYY-MM-DDThh:mm:ss.sss<sss>. UTC
	int wimg, himg;			/// 图像像素数
	double expdur;			/// 曝光时间, 量纲: 秒
	/* 目标提取结果 */
	double fwhm;			/// 统计半高全宽
	/* 天文定位结果 */
	point_2f coordCenter;	/// 视场中心赤道坐标, 量纲: 角度, 坐标系: J2000
	//...WCS信息
	/* 天文测光结果 */
	CeleBodyVec bodies;		/// 从图像中提取的天体集合
};
typedef boost::shared_ptr<ImageFrame> ImgFrmPtr;
typedef std::deque<ImgFrmPtr> ImgFrmDeque;

#endif
