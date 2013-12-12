/* 
 * File:   SocketClient.h
 * Author: khoai
 *
 * Created on November 26, 2013, 3:18 PM
 */

#ifndef SOCKETCLIENT_H
#define	SOCKETCLIENT_H

#include <iostream>
#include <string.h>
#include <fstream>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "HttpHeader.h"

#define MSG_LEN 512
#define MAX_HEADER_SIZE 4096
#define MAX_ADDR_LEN 512
#define MINI_LEN 128
#define MAX_PACKET_LEN 65536
#define MAX_FILE_PATH_LEN 260

using namespace std;

class SocketClient {
	struct hostent *remoteHost;
	struct sockaddr_in serverAddress;
	int sock;
public:
	bool isConnect;

	SocketClient() {
		isConnect = false;
	}

	SocketClient(string sHostAddress, int port) {
		isConnect = InitConnection(sHostAddress, port);
	}

	int Send(char* msg) {
		return send(sock, msg, strlen(msg), 0);
	}

	int Send(string msg) {
		return Send(msg.c_str());
	}

	int Receive(char* buffer, int maxLen) {
		int received = recv(sock, buffer, maxLen, 0);

		if (received <= 0) {
			perror("Fail, error code: received <= 0");
			cout << endl;
		}
		return received;
	}

	void CloseSocket() {
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}

private:

	bool InitConnection(string sHostAddress, int port) {
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (this->sock < 0)
			perror("ERROR opening socket");

		remoteHost = gethostbyname(sHostAddress.c_str());
		if (this->remoteHost == NULL) {
			fprintf(stderr, "ERROR, no such host\n");
			return false;
		}
		bzero((char *) &serverAddress, sizeof (serverAddress));
		serverAddress.sin_family = AF_INET;
		bcopy((char *) remoteHost->h_addr,
				(char *) &serverAddress.sin_addr.s_addr,
				remoteHost->h_length);
		serverAddress.sin_port = htons(port);
		if (connect(sock, (struct sockaddr *) &serverAddress, sizeof (serverAddress)) < 0) {
			perror("ERROR connecting");
			return false;
		}

		return true;
	}
};


#endif	/* SOCKETCLIENT_H */

