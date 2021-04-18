/*!
 Name        : Astronomical Digital Image Process Software
 Author      : Xiaomeng Lu
 Version     : 0.1
 */

#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/filesystem.hpp>
#include "Parameter.hpp"
#include "GLog.h"
#include "FITSHandlerImage.hpp"
#include "ImageFrame.hpp"

///////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::filesystem;

typedef vector<string> strvec;

GLog _gLog(stdout);
///////////////////////////////////////////////////////////////////////
/*!
 * @brief 显示使用说明
 */
void Usage() {
	printf("Usage:\n");
	printf(" adips [options] [<image file 1> <image file 2> ...]\n");
	printf("\nOptions\n");
	printf(" -h / --help    : print this help message\n");
	printf(" -d / --default : generate default configuration file here\n");
	printf(" -c / --config  : specify configuration file\n");
	printf(" -Z / --zero    : combine bias images, result to be saved as ZERO.fits in WD\n");
	printf(" -D / --dark    : combine dark images, result to be saved as DARK.fits in WD\n");
	printf(" -F / --flat    : combine flat images, result to be saved as FLAT.fits in WD\n");
}

void process_sequence(strvec& imgFiles, Parameter* param) {
	int n;
	if ((n = imgFiles.size()) > 2) {
		sort(imgFiles.begin(), imgFiles.end(), [](const string &name1, const string &name2) {
			return name1 < name2;
		});
	}

	int errcode;
	FITSHandlerImage imgFITS;
	ImgFrmPtr imgFrmPtr;
	path pathFile;
	double expdur;

	for (int i = 0; i < n; ++i) {
		pathFile = imgFiles[i];
		// 打开FITS文件, 读取关键头信息和数据
		errcode = imgFITS.LoadImage(imgFiles[i].c_str());
		if (errcode) {
			_gLog.Write(LOG_FAULT, "image file [%s]: %s",
					imgFiles[i].c_str(),
					errcode == 1 ? "failed to open" :
							(errcode == 2 ? "not found necessary keywords" :
									"failed to read image data"));
			continue;
		}
		// 图像处理, 提取目标及目标特征
		imgFrmPtr = make_image_frame();
		imgFrmPtr->filename = pathFile.filename().string();
		imgFrmPtr->dateobs = imgFITS.GetExptime(expdur);
		imgFrmPtr->expdur = expdur;
		//...天文定位
		if (param->useAstrometry || param->usePhotometry || param->useMotionAss) {

		}
		//...测光
		if (param->usePhotometry || param->useMotionAss) {

		}
		//...关联识别运动目标
		if (param->useMotionAss) {

		}
		//...输出处理结果
	}
}

void combine_images(strvec& imgFiles, Parameter* param, int combine) {
	int n;
	if ((n = imgFiles.size()) <= 3) {
		_gLog.Write(LOG_WARN, "combine operation needs at least 4 frames image");
	}
	else {
		FITSHandlerImage *imgFITSs = new FITSHandlerImage[n];
//		int w, h;

		delete []imgFITSs;
	}
}

void process_directory(const string& dirname, Parameter* param, int combine) {
	strvec imgFiles;

	for (directory_iterator x = directory_iterator(dirname); x != directory_iterator(); ++x) {
		if (is_directory(x->path().string())) process_directory(x->path().string(), param, combine);
		else if (x->path().extension().string().rfind(".fit") != string::npos) {
			imgFiles.push_back(x->path().string());
		}
	}
	if (imgFiles.size()) {
		if (!combine)
			process_sequence(imgFiles, param);
		else
			combine_images(imgFiles, param, combine);
	}
}

int main(int argc, char** argv) {
	/* 解析命令行参数 */
	struct option longopts[] = {
		{ "help",    no_argument,       NULL, 'h' },
		{ "default", no_argument,       NULL, 'd' },
		{ "config",  required_argument, NULL, 'c' },
		{ "zero",    no_argument,       NULL, 'Z' },
		{ "dark",    no_argument,       NULL, 'D' },
		{ "flat",    no_argument,       NULL, 'F' },
		{ NULL,      0,                 NULL,  0  }
	};
	char optstr[] = "hdc:ZDF";
	int ch, optndx;
	int combine(0);
	Parameter param;

	while ((ch = getopt_long(argc, argv, optstr, longopts, &optndx)) != -1) {
		switch(ch) {
		case 'h':
			Usage();
			return -1;
		case 'd':
			cout << "generating default configuration file here" << endl;
			param.Init("adips.xml");
			return -2;
		case 'c':
			if (!param.Load(argv[optind - 1])) {
				_gLog.Write(LOG_FAULT, "failed to load configuration file [%s]", argv[optind - 1]);
				return -3;
			}
			break;
		case 'Z':
			combine = 1;
			break;
		case 'D':
			combine = 2;
			break;
		case 'F':
			combine = 3;
			break;
		default:
			Usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!argc) {
		_gLog.Write(LOG_WARN, "require FITS file(s) which to be processed");
		return -4;
	}

	/////////////////////////////////////////////////////////////////////////
	strvec imgFiles;

	for (int i = 0; i < argc; ++i) {
		path filename = argv[i];
		if (is_regular_file(filename) && filename.extension().string().rfind(".fit") != string::npos)
			imgFiles.push_back(argv[i]);
		else if (is_directory(filename))
			process_directory(filename.string(), &param, combine);
	}
	if (imgFiles.size()) {
		if (!combine)
			process_sequence(imgFiles, &param);
		else
			combine_images(imgFiles, &param, combine);
	}

	return 0;
}
