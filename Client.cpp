#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>
#include <ctime>
#include <future>
#include <atomic>
#include <chrono>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 128
#define DEFAULT_PORT_F "27014"
#define DEFAULT_PORT_G "27015"


std::string connectToServer(PCSTR port_,int argc, char **argv, std::string message) {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char sendbuf [DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	int n = message.length();

	for (int i = 0; i < n; i++) {
		sendbuf[i] = message[i];
	}

	// Validate the parameters
	if (argc != 2) {
		throw "usage: %s server-name\n", argv[0];
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		throw "WSAStartup failed with error: %d\n", iResult;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], port_, &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		throw "getaddrinfo failed with error: %d\n", iResult;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			WSACleanup();
			throw "socket failed with error: %ld\n", WSAGetLastError();
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		WSACleanup();
		throw "Unable to connect to server!\n";
	}

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		throw "send failed with error: %d\n", WSAGetLastError();
	}


	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		throw "shutdown failed with error: %d\n", WSAGetLastError();
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			
			for (int i = 0; i < n; i++) {
				message[i] = recvbuf[i];
			}

			do {
				iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			} while (iResult > 0);

		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return message;
}


int __cdecl main(int argc, char **argv)
{

	std::string message;
	std::getline(std::cin, message);

	std::string resF, resG;

	std::atomic<bool> doneF(false); // Use an atomic flag.
	std::thread f([&doneF, argc, argv, &message, &resF] {
		resF = connectToServer("27014", argc, argv, message);
		doneF = true;
	});

	std::atomic<bool> doneG(false); // Use an atomic flag.
	std::thread g([&doneG, argc, argv, &message, &resG] {
		resG = connectToServer("27015", argc, argv, message);
		doneG = true;
	});

	bool noAsk = false;
	bool isStop = false;
	while (true) {

		time_t start, end;
		double elapsed;  // seconds
		start = time(NULL);
		int terminate = 1;
		while (terminate) {
			end = time(NULL);
			elapsed = difftime(end, start);

			if (doneF) {
				break;
			}
			std::cout << elapsed << std::endl;
			if (elapsed >= 2.0)
				terminate = 0;
		}
		if (!doneF && doneG) {
			std::cout << "we can get by 'g' function result\nStop it? " << std::endl;
			std::string ans;
			while (true) {
				std::getline(std::cin, ans);
				if (ans == "yes") {
					isStop = true;
					break;
				}
				else if (ans == "no") {
					break;
				}
			}
		}else if (!noAsk) {
			std::cout << "Function do work more than 10 seconds. Try again?" << std::endl;
			std::string ans;
			while (true) {
				std::getline(std::cin, ans);
				std::cout << ans << std::endl;
				if (ans == "yes") {
					break;
				}
				else if (ans.compare("no") == 0) {
					isStop = true;
					break;
				}
				else if (ans == "noAsk") {
					noAsk = true;
					break;
				}
			}
		}
		else if (doneF || doneG) break;
		if (isStop) break;
	}
	std::cout << "Stoping" << std::endl;
	if (!isStop || doneG) {
		if (doneF) std::cout << "ResF is:\t" << resF << std::endl;
		else if (doneG) std::cout << "ResG is:\t" << resG << std::endl;
	}
	if(f.joinable()) f.join();
	if(g.joinable()) g.join();
	


	return 0;
}