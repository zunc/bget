/* 
 * File:   Util.h
 * Author: khoai
 *
 * Created on November 26, 2013, 4:26 PM
 */

#ifndef UTIL_H
#define	UTIL_H
#include <sys/stat.h>

class Util {
public:

	static int revStrCmp(char* str1, const char* str2) {
		int lenStr1 = strlen(str1);
		int lenStr2 = strlen(str2);
		for (int i = lenStr2 - 1; i >= 0; i--) {
			if (str2[i] == str1[lenStr1 - lenStr2 + i])
				continue;
			else
				return (str2[i] - str1[lenStr1 - lenStr2 - i]);
		}
		return 0;
	}

	static long roundIncreaseKb(long size) {
		// size in byte
		return ((int) (size / 1024) + 1) * 1024;
	}

	static string resolveDirectory(string path) {
		string sDir = "";
		int posSplash = path.find('/');
		if (posSplash > 0)
			sDir = path.substr(0, posSplash);
		return sDir;
	}

	static string resolveFileName(string path) {
		string sFile = path;
		int posSplash = path.rfind('/');
		if (posSplash > 0)
			sFile = path.substr(posSplash, path.length() - posSplash);
		return sFile;
	}

	static int mkDir(string sDir) {
		return mkdir(sDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	static int rm(string sFile) {
		return remove(sFile.c_str());
	}

	static size_t appendCopy(string dst, string src) {
		FILE* fDst = fopen(dst.c_str(), "ab");
		if (fDst == NULL) {
			perror("can't open");
			return 0;
		}

		FILE* fSrc = fopen(src.c_str(), "rb");
		if (fSrc == NULL) {
			perror("can't open");
			fclose(fDst);
			return 0;
		}

		char szBuffer[1024] = {0};
		size_t count = 0;
		while (true) {
			size_t size = fread(szBuffer, 1, sizeof (szBuffer), fSrc);
			count += size;
			if (size <= 0)
				break;
			fwrite(szBuffer, 1, sizeof (szBuffer), fDst);
		}

		// release handle
		fclose(fSrc);
		fclose(fDst);

		return count;
	}

	static size_t joinFile(string dst, string preNameTemp, size_t nPart, bool isRemoveTemp) {
		char szFilePart[1024] = {0};
		size_t file_size = 0;
		for (int i = 0; i < nPart; i++) {
			sprintf(szFilePart, "%s.part%d", preNameTemp.c_str(), i);
			size_t size = appendCopy(dst, szFilePart);
			if (size > 0) {
				file_size += size;
				if (isRemoveTemp)
					rm(szFilePart);
			} else {
				cout << "error at path: " << szFilePart << endl;
				return 0;
			}
		}
		return file_size;
	}
};

#endif	/* UTIL_H */

