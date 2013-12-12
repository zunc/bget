/* 
 * File:   HttpClient.h
 * Author: khoai
 *
 * Created on November 26, 2013, 4:24 PM
 */

#ifndef HTTPCLIENT_H
#define	HTTPCLIENT_H

#include <pthread.h>

#include "SocketClient.h"
#include "Util.h"

#define DEBUG_SHOW_HEADER 0

class HttpClient {
	//    SocketClient* client;
	//    string sUrl;

public:

	HttpClient() {

	}

	HttpHeader* getHeader(string sUrl) {
		HttpHeader httpHeader;
		string sHostAddress, sFileAddressRelative, sFileName;
		int port = 80;
		ResolveUrl(sUrl, sHostAddress, sFileAddressRelative, sFileName, port);

		SocketClient client = SocketClient(sHostAddress, port);
		if (!client.isConnect)
			return NULL;

		httpHeader.Add("Host", string(sHostAddress));
		httpHeader.Add("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

		char msg[MSG_LEN] = {0};
		sprintf(msg, "GET %s HTTP/1.1\r\n%s", sFileAddressRelative.c_str(), httpHeader.ToString().c_str());
		int result = client.Send(msg);

		char szHeader[MAX_HEADER_SIZE] = {0};
		int received;
		received = ReceiveHeader(client, szHeader, MAX_PACKET_LEN);
		if (received <= 0) {
			cout << "Get http header -> error" << endl;
			client.CloseSocket();
			return NULL;
		}

		//--- get header
		HttpHeader* responseHeader = new HttpHeader(string(szHeader));
		if (responseHeader->getHttpCode() != 200) {
			cout << "Can't download file: " << responseHeader->getHttpMessage() << endl;
		}

#if DEBUG_SHOW_HEADER
		cout << "---\n" << responseHeader->ToString() << endl;
#endif

		client.CloseSocket();
		return responseHeader;
	}

	void DownloadByRange(string sUrl, int offset, int len, string dstFile) {
		HttpHeader httpHeader;
		string sHostAddress, sFileAddressRelative, sFileName;
		int port = 80;
		ResolveUrl(sUrl, sHostAddress, sFileAddressRelative, sFileName, port);

		SocketClient client = SocketClient(sHostAddress, port);

		// build request header
		char sRange[MINI_LEN] = {0};
		sprintf(sRange, "bytes=%d-%d", offset, offset + len - 1);
		httpHeader.Add("Host", string(sHostAddress));
		httpHeader.Add("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");
		httpHeader.Add("Range", sRange);

		char msg[MSG_LEN] = {0};
		sprintf(msg, "GET %s HTTP/1.1\r\n%s", sFileAddressRelative.c_str(), httpHeader.ToString().c_str());
		int result = client.Send(msg);

		// get response header
		char szHeader[MAX_HEADER_SIZE] = {0};
		int received;
		received = ReceiveHeader(client, szHeader, MAX_PACKET_LEN);
		if (received <= 0) {
			cout << "Get http header -> fail" << endl;
			client.CloseSocket();
			return;
		}

		//--- get file
		HttpHeader responseHeader = HttpHeader(string(szHeader));
		if (responseHeader.getHttpCode() != 206) {
			cout << "Can't download file: " << responseHeader.getHttpMessage() << endl;
			client.CloseSocket();
			return;
		}

		string sContentLength = responseHeader.getValue("Content-Length");
		string sContentType = responseHeader.getValue("Content-Type");
		int contentLength = atoi(sContentLength.c_str());

		ofstream of;
		of.open(dstFile.c_str(), ios::out | ios::binary);

		if (!of.is_open()) {
			cout << "Can not open file for writing" << endl;
			client.CloseSocket();
			return;
		}

		char szData[MAX_PACKET_LEN] = {0};
		int totalReceived = 0;

		// --- download file
		int receivedInByte = 0;
		while (true) {
			int buffSize = MAX_PACKET_LEN;
			int remain = contentLength - totalReceived;
			if (buffSize > remain)
				buffSize = remain;

			received = client.Receive(szData, buffSize);
			if (received <= 0)
				break;

			receivedInByte += received;

			totalReceived += received;

			of.write(szData, received);
			of.flush();

			if (totalReceived == contentLength) {
				break;
			}
		}

		of.close();
		client.CloseSocket();
	}

	struct DownloadInfo {
		string sUrl;
		string sFile;
		int offset;
		int len;
	};

	static void* DownloadByRangeThread(void *pData) {
		DownloadInfo *info = (DownloadInfo*) pData;

		HttpHeader httpHeader;
		string sHostAddress, sFileAddressRelative, sFileName;
		int port = 80;
		ResolveUrl(info->sUrl, sHostAddress, sFileAddressRelative, sFileName, port);

		SocketClient client = SocketClient(sHostAddress, port);

		// build request header
		char sRange[MINI_LEN] = {0};
		sprintf(sRange, "bytes=%d-%d", info->offset, info->offset + info->len - 1);
		httpHeader.Add("Host", string(sHostAddress));
		httpHeader.Add("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");
		httpHeader.Add("Range", sRange);

		char msg[MSG_LEN] = {0};
		sprintf(msg, "GET %s HTTP/1.1\r\n%s", sFileAddressRelative.c_str(), httpHeader.ToString().c_str());
		int result = client.Send(msg);

		// get response header
		char szHeader[MAX_HEADER_SIZE] = {0};
		int received;
		received = ReceiveHeader(client, szHeader, MAX_PACKET_LEN);
		if (received <= 0) {
			cout << "Get http header -> fail" << endl;
			client.CloseSocket();
			return NULL;
		}

		//--- get file
		HttpHeader responseHeader = HttpHeader(string(szHeader));
		if (responseHeader.getHttpCode() != 206) {
			cout << "Can't download file: " << responseHeader.getHttpMessage() << endl;
			client.CloseSocket();
			return NULL;
		}

#if DEBUG_SHOW_HEADER
		cout << "--- " << responseHeader.getHttpCode() << "\n" <<
				responseHeader.ToString() << endl;
#endif

		string sContentLength = responseHeader.getValue("Content-Length");
		string sContentType = responseHeader.getValue("Content-Type");
		int contentLength = atoi(sContentLength.c_str());

		ofstream of;
		of.open(info->sFile.c_str(), ios::out | ios::binary);

		if (!of.is_open()) {
			cout << "Can not open file for writing" << endl;
			client.CloseSocket();
			return NULL;
		}

		char szData[MAX_PACKET_LEN] = {0};
		int totalReceived = 0;

		// --- download file
		int receivedInByte = 0;
		while (true) {
			int buffSize = MAX_PACKET_LEN;
			int remain = contentLength - totalReceived;
			if (buffSize > remain)
				buffSize = remain;

			received = client.Receive(szData, buffSize);
			if (received <= 0)
				break;

			receivedInByte += received;
			totalReceived += received;

			of.write(szData, received);
			of.flush();

			if (totalReceived == contentLength) {
				break;
			}
		}

		of.close();
		client.CloseSocket();

		cout << "Downloaded: " << info->sFile << endl;
	}
	
	bool DownloadFileMultiThread(string sUrl, string dstFile, int nThread) {
		//--- advance download
		// one request for get header
		// another request for get file content, multi thread download if server support
		bool retFlag = false;
		
		string sHostAddress, sFileAddressRelative, sFileName;
		int port = 80;
		ResolveUrl(sUrl, sHostAddress, sFileAddressRelative, sFileName, port);

		//--- get http header
		HttpHeader* responseHeader = getHeader(sUrl);
		if (responseHeader == NULL)
			return retFlag;

		string sContentLength = responseHeader->getValue("Content-Length");
		string sContentType = responseHeader->getValue("Content-Type");
		int contentLength = atoi(sContentLength.c_str());
		
		// --- init path
		string sTempDir = Util::resolveDirectory(dstFile);
		if (!sTempDir.empty())
			sTempDir += "/";
		sTempDir += ".btmp";
		Util::mkDir(sTempDir);
		string sFileTemp = sTempDir + "/" + Util::resolveFileName(sFileName);
		
		// print basic information
		cout << "--- File information" << endl;
		cout << "\tName: " << dstFile << endl;
		cout << "\tTemp files: " << sFileTemp << ".partN" << endl;
		cout << "\tSize: " << sContentLength << endl;
		cout << "\tType: " << sContentType << endl;
		cout << endl;
		
		// check server support resume
		string rangeSection = responseHeader->getValue("Accept-Ranges");
		delete responseHeader;
		if (rangeSection != "") {
			cout << " -> Server support resume/multithread download" << endl;
		} else {
			cout << " -> Server don't support resume/multithread download" << endl;
			return retFlag;
		}
		
		long PART_SIZE = 100 * 1024 * 1024;
		int currentPos = 0;
		int countPart = 0;
		
		int N_THREAD = nThread;

		pthread_t thread[N_THREAD];
		DownloadInfo dInfo[N_THREAD];
		
		PART_SIZE = Util::roundIncreaseKb((long)(contentLength / N_THREAD));
		
		// --- init thread
		while (currentPos < contentLength) {
			int nextPartSize = PART_SIZE;
			if ((currentPos + PART_SIZE) > contentLength)
				nextPartSize = contentLength - currentPos;
			
			char szFilePart[MAX_FILE_PATH_LEN] = {0};
			sprintf(szFilePart, "%s.part%d", sFileTemp.c_str(), countPart);
			
			dInfo[countPart].sUrl = sUrl;
			dInfo[countPart].sFile = string(szFilePart);
			dInfo[countPart].offset = currentPos;
			dInfo[countPart].len = nextPartSize;

			int ret = pthread_create(&thread[countPart], NULL, DownloadByRangeThread, &dInfo[countPart]);
			if (ret != 0) {
				perror("pthread_create fail");
				retFlag = false;
			}

			currentPos += nextPartSize;
			countPart++;
		}
		
		// --- waiting thread
		for (int i = 0; i < countPart; i++) {
			pthread_join(thread[i], NULL);
		}
		
		// --- merge file
		size_t file_size = Util::joinFile(dstFile, sFileTemp, countPart, true);
		if (file_size > 0) {
			retFlag = true;
			Util::rm(sTempDir);
		}
		
		return retFlag;
	};

	void DownloadFileAdvance(string sUrl, string dstDir) {
		//--- advance download
		// one request for get header
		// another request for get file content, multi thread download if server support

		string sHostAddress, sFileAddressRelative, sFileName;
		int port = 80;
		ResolveUrl(sUrl, sHostAddress, sFileAddressRelative, sFileName, port);

		//--- get http header
		HttpHeader* responseHeader = getHeader(sUrl);
		if (responseHeader == NULL)
			return;

		string sContentLength = responseHeader->getValue("Content-Length");
		string sContentType = responseHeader->getValue("Content-Type");
		int contentLength = atoi(sContentLength.c_str());
		string sFilePath = dstDir + sFileName;

		// print basic information
		cout << "\tName: " << sFilePath << endl;
		cout << "\tSize: " << sContentLength << endl;
		cout << "\tType: " << sContentType << endl;
		cout << endl;

		// check server support resume
		string rangeSection = responseHeader->getValue("Accept-Ranges");
		if (rangeSection != "") {
			cout << " -> Server support resume download" << endl;
		} else {
			cout << " -> Server don't support resume download" << endl;
			delete responseHeader;
			return;
		}

		int PART_SIZE = 128 * 1024 * 1024;
		int currentPos = 0;
		int count = 1;
		while (currentPos < contentLength) {
			int nextPartSize = PART_SIZE;
			if ((currentPos + PART_SIZE) > contentLength)
				nextPartSize = contentLength - currentPos;

			char szFilePart[MAX_FILE_PATH_LEN] = {0};
			sprintf(szFilePart, "%s.part%d", sFilePath.c_str(), count);
			DownloadByRange(sUrl, currentPos, nextPartSize, szFilePart);

			currentPos += nextPartSize;
			cout << "Downloaded part: " << count << " , size: " << nextPartSize << endl;
			count++;
		}

		delete responseHeader;
	};

	bool DownloadFileBasic(string sUrl, string dstFile) {
		//--- basic download
		// one request for get all: header, file content
		bool retFlag = false;
		
		//--- get http header
		HttpHeader httpHeader;
		string sHostAddress, sFileAddressRelative, sFileName;
		int port = 80;
		ResolveUrl(sUrl, sHostAddress, sFileAddressRelative, sFileName, port);

		SocketClient client = SocketClient(sHostAddress, port);

		httpHeader.Add("Host", string(sHostAddress));
		httpHeader.Add("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

		char msg[MSG_LEN] = {0};
		sprintf(msg, "GET %s HTTP/1.1\r\n%s", sFileAddressRelative.c_str(), httpHeader.ToString().c_str());
		int result = client.Send(msg);

		char szHeader[MAX_HEADER_SIZE] = {0};
		int received;
		received = ReceiveHeader(client, szHeader, MAX_PACKET_LEN);
		if (received <= 0) {
			cout << "Get http header -> fail" << endl;
			client.CloseSocket();
			return retFlag;
		}

		//--- get file
		HttpHeader responseHeader = HttpHeader(string(szHeader));
		int httpCode = responseHeader.getHttpCode();
		if (httpCode != 200) {
			cout << "Can't download file: " << responseHeader.getHttpMessage() << endl;
			client.CloseSocket();
			return retFlag;
		}
		
		string sContentLength = responseHeader.getValue("Content-Length");
		string sContentType = responseHeader.getValue("Content-Type");
		int contentLength = atoi(sContentLength.c_str());
		string sFilePath = dstFile;
		
		if (contentLength <= 0) {
			cout << "error. file contentLength == 0" << endl;
			return retFlag;
		}
		
		// print basic information
		cout << "\tName: " << sFilePath << endl;
		cout << "\tSize: " << sContentLength << endl;
		cout << "\tType: " << sContentType << endl;
		cout << endl;

		ofstream of;
		of.open(sFilePath.c_str(), ios::out | ios::binary);

		if (!of.is_open()) {
			cout << "Can not open file for writing" << endl;
			cout << endl;
			client.CloseSocket();
			return retFlag;
		}

		char szData[MAX_PACKET_LEN] = {0};
		int totalReceived = 0;
		int previousPercent = 0;

		// --- download
		time_t t = time(NULL);
		int receivedInByte = 0;
		while ((received = client.Receive(szData, MAX_PACKET_LEN)) > 0) {

			receivedInByte += received;

			totalReceived += received;
			int currentPercent = totalReceived * 100 / contentLength;

			cout << "\tFile size : " << (contentLength / 1024) << " kB" << endl;
			cout << "\tDownloaded: " << (totalReceived / 1024) << " kB (" << currentPercent << "%)" << endl;

			of.write(szData, received);
			of.flush();

			if (totalReceived == contentLength) {
				break;
			}
		}

		of.close();
		if (totalReceived != contentLength) {
			cout << " -> Fail, no error occured =.=" << endl;
			cout << endl;
			retFlag = false;
		} else {
			cout << " -> Succeed" << endl;
			cout << endl;
			retFlag = true;
		}

		client.CloseSocket();
		return retFlag;
	}
	
	static string ResolveFileNameOfUrl(string sUrl) {
		string sHostAddress, sFileAddressRelative, sFileName;
		int port = 80;
		ResolveUrl(sUrl, sHostAddress, sFileAddressRelative, sFileName, port);
		return sFileName;
	}
	
private:

	static void ResolveUrl(string szUrl, string &sHostAddress, string &sFileAddressRelative,
			string &sAutoFileName, int &port) {
		string url(szUrl);

		port = 80;
		int protocolPos = url.find("://");
		if (protocolPos != string::npos) {
			url.replace(0, protocolPos + strlen("://"), "");
		}

		int firstSlashPos = url.find("/");
		if (firstSlashPos == string::npos) {
			sHostAddress = url;
			sFileAddressRelative = "/";
			sAutoFileName = "index.html";
		} else {
			sHostAddress = url.substr(0, firstSlashPos);

			int colonPos = sHostAddress.find(':');
			if (colonPos > 0) {
				string sPort = sHostAddress.substr(colonPos + 1, sHostAddress.length() - colonPos - 1);
				port = atoi(sPort.c_str());
				sHostAddress = sHostAddress.substr(0, colonPos);
			}

			string fileAddressRelative = url.substr(firstSlashPos, url.length() - firstSlashPos + 1);
			sFileAddressRelative = fileAddressRelative;

			int lastSlashPos = url.find_last_of('/');
			string autoFileName = url.substr(lastSlashPos + 1, url.length() - firstSlashPos);
			sAutoFileName = autoFileName;
		}
	}
	
	void GetValueFromHeader(char *szHeader, char* szValueName, char *szValue) {
		string header(szHeader);
		int valuePos = header.find(szValueName);
		if (valuePos == string::npos) {
			szValue[0] = '\0';
		} else {
			int endLinePos = header.find('\n', valuePos);
			string value = header.substr(valuePos + strlen(szValueName) + strlen(": "), endLinePos - valuePos - strlen(szValueName) - strlen(": "));
			strcpy(szValue, value.c_str());
		}
	}

	static int ReceiveHeader(SocketClient client, char* buffer, int maxLen) {

		char bData[2] = {0};

		int nRecv = 0;
		for (int i = 0; i < maxLen; i++) {
			nRecv++;
			int received = client.Receive(bData, 1);
			if (received <= 0) {
				break;
			}

			buffer[i] = bData[0];
			if (bData[0] == '\n') {
				// check last byte of header
				if (i > 4) {
					if (Util::revStrCmp(buffer, "\r\n\r\n") == 0) {
						// yeah, in the end
						break;
					}
				}
			}
		}
		return nRecv;
	}
};
#endif	/* HTTPCLIENT_H */

