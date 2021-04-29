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
#include <boost/asio.hpp>
#include "Parameter.hpp"
#include "GLog.h"
#include "ADIWorkFlow.h"

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
	ADIWorkFlow workFlow;
	boost::asio::io_service ios;
	boost::asio::signal_set signals(ios, SIGINT, SIGTERM);  // interrupt signal
	printf ("*************************************\n");
	printf ("*                                   *\n");
	printf ("* press Ctrl+C to terminate program *\n");
	printf ("*                                   *\n");
	printf ("*************************************\n");

	signals.async_wait(boost::bind(&boost::asio::io_service::stop, &ios));

	if (!workFlow.Start(param)) {
		_gLog.Write(LOG_FAULT, "failed to start process procedure");
	}
	else {
		int i, n;
		if ((n = imgFiles.size()) > 2) {
			sort(imgFiles.begin(), imgFiles.end(), [](const string &name1, const string &name2) {
				return name1 < name2;
			});
		}
		for (i = 0; i < n; ++i) {
			workFlow.ProcessImage(imgFiles[i].c_str());
		}
		ios.run();
		workFlow.Stop();
	}
}

void combine_images(strvec& imgFiles, Parameter* param, int combine) {
	int i, n;

	if ((n = imgFiles.size()) <= 3) {
		_gLog.Write(LOG_WARN, "combine operation needs at least 4 frames image");
	}
	else {
		ADIWorkFlow workFlow;

		workFlow.BeginCombine(param, combine);
		for (i = 0; i < n; ++i)
			workFlow.AddCombineImage(imgFiles[i].c_str());
		workFlow.EndCombine();
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
	bool loadParam(false);
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
			loadParam = true;
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
	if (!loadParam) {
		if (!param.Load("adips.xml") && !param.Load("/usr/local/etc/adips.xml")) {
			_gLog.Write(LOG_FAULT, "failed to load configuration file [%s] and [%s]",
					"adips.xml", "/usr/local/etc/adips.xml");
			return -5;
		}
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
