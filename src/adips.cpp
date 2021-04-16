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
#include "Parameter.hpp"

using namespace std;
typedef vector<string> strvec;

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
	printf(" -Z / --zero    : combine bias images\n");
	printf(" -D / --dark    : combine dark images\n");
	printf(" -F / --flat    : combine flat images\n");
}

int main(int argc, char** argv) {
	/* 解析命令行参数 */
	struct option longopts[] = {
		{ "help",    no_argument,       NULL, 'h' },
		{ "default", no_argument,       NULL, 'd' },
		{ "config",  required_argument, NULL, 'c' },
		{ "zero",    required_argument, NULL, 'Z' },
		{ "dark",    required_argument, NULL, 'D' },
		{ "flat",    required_argument, NULL, 'F' },
		{ NULL,      0,                 NULL,  0  }
	};
	char optstr[] = "hdc:Z:D:F:";
	int ch, optndx;
	bool combineZero(false), combineDark(false), combineFlat(false);
	string pathParam;
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
			pathParam = argv[optind - 1];
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
		printf("requires FITS file which to be processed\n");
		Usage();
		return -3;
	}

	/////////////////////////////////////////////////////////////////////////
	// 缓存文件
	// 文件名增量排序
	// 启动处理

	return 0;
}
