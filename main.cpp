/* 
 * File:   main.cpp
 * Author: khoai
 *
 * Created on November 26, 2013, 3:08 PM
 */

#include <iostream>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include "HttpClient.h"

static void show_help(void) {
	const char *b = " [^-^] bget 0.1 - a multithread downloader\n" \
		"usage:\n" \
		" -t,\t --thread=NUMBER\t thread count (default = 5)\n" \
		" -o,\t --output=FILE\t\t destination of output file\n" \
		" -h,\t --help\t\t\t show help\n" \
	;
	cout << b << endl;
}

int main(int argc, char **argv) {
	int c;
	const char* url = NULL;
	const char* dst = NULL;
	int nThread = 5;
	bool isErrorOpt = false;
	bool isResumeDownload = false;
	
	static struct option long_options[] = {
		{"thread",	required_argument,	0, 't'},
		{"output",	required_argument,	0, 'o'},
		{"help",	no_argument,		0, 'h'},
		/* {"resume",	no_argument,		0, 'r'}, */ 
		{NULL, 0, NULL, 0}
	};
	
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "ht:o:",
			long_options, &option_index)) != -1) {
		switch (c) {
			case 't':
				if (optarg == NULL) {
					cout << "param: -t is NULL" << endl;
					exit(EXIT_FAILURE);
				}
				nThread = atoi(optarg);
				break;
			case 'o':
				if (optarg == NULL) {
					cout << "param: -o is NULL" << endl;
					exit(EXIT_FAILURE);
				}
				dst = strdup((const char*) optarg);
				break;
			case 'h':
				show_help();
				exit(EXIT_FAILURE);
				break;
			/* case 'r':
				isResumeDownload = true;
				break; */
			default:
				cout << "incorrect parameter, bye bye x,x" << endl;
				cout << "\n>--- F1 for you" << endl;
				show_help();
				exit(EXIT_FAILURE);
				break;
		}
	}
	
	if (optind < argc) {
		url = strdup(argv[optind]);
	} else {
		cout << "not found url parameter" << endl;
		isErrorOpt = true;
	}
	
	if (isErrorOpt)
		exit(EXIT_FAILURE);

	string sDst = "";
	if (dst == NULL)
		sDst = HttpClient::ResolveFileNameOfUrl(url);
	else
		sDst = string(dst);
	
	HttpClient client;
	bool retFlag = false;
	if (nThread == 1) {
		retFlag = client.DownloadFileBasic(url, sDst);
	} else if (nThread > 1) {
		retFlag = client.DownloadFileMultiThread(url, sDst, nThread);
	}
	
	if (retFlag) {
		cout << "> Download success, goodbye :*" << endl;
	}
}

