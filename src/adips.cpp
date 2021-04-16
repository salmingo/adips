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
#include <boost/filesystem.hpp>
#include "Parameter.hpp"
#include "GLog.h"

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
	bool combineZero(false), combineDark(false), combineFlat(false);
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
			combineZero = true;
			break;
		case 'D':
			combineDark = true;
			break;
		case 'F':
			combineFlat = true;
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
	}

	if (combineZero || combineDark || combineFlat) {

	}
	else {

	}
	// 缓存文件
	// 文件名增量排序
	// 启动处理

	return 0;
}
