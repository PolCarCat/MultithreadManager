﻿#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define LISTEN_PORT 8888

#define PAUSE_AND_EXIT() system("pause"); exit(-1)

void printWSErrorAndExit(const char *msg)
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	PAUSE_AND_EXIT();
}

void server(int port)
{
	// TODO-1: Winsock init

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NO_ERROR) {
		printWSErrorAndExit("SERVER ERROR");

	}



	// TODO-2: Create socket (IPv4, stream, TCP)

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);


	// TODO-3: Configure socket for address reuse

	iResult = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int));

	if (iResult == SOCKET_ERROR)
	{
		printWSErrorAndExit("SERVER SOCKET ERROR");
	}

	// TODO-4: Create an address object with any local address

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(LISTEN_PORT);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	// TODO-5: Bind socket to the local address

	iResult = bind(s, (const sockaddr*)&bindAddr, sizeof(bindAddr));

	// TODO-6: Make the socket enter into listen mode

	iResult = listen(s, 3);

	// TODO-7: Accept a new incoming connection from a remote host
	// Note that once a new connection is accepted, we will have
	// a new socket directly connected to the remote host.

	SOCKET accepted;
	struct sockaddr_in client;

	int size = sizeof(client);
	accepted = accept(s, (struct sockaddr*)&client, &size);

	while (true)
	{
		// TODO-8:
		// - Wait a 'ping' packet from the client
		// - Send a 'pong' packet to the client
		// - Control errors in both cases

		// - Wait a 'ping' packet from the client

		char recieved[10];

		iResult = recv(accepted, (char*)recieved, 10, 0);

		if (iResult == SOCKET_ERROR)
		{
			printWSErrorAndExit("SERVER SOCKET ERROR");
		}
		else
		{
			printf(recieved);
		}

		char* pong = "pong";

		iResult = send(accepted,pong ,sizeof pong, 0);

		if (iResult == SOCKET_ERROR)
		{
			printWSErrorAndExit("SERVER SOCKET ERROR");
		}
	}

	// TODO-9: Close socket
	
	int closesocket(s);


	// TODO-10: Winsock shutdown


	iResult = WSACleanup();

}

int main(int argc, char **argv)
{
	server(LISTEN_PORT);

	PAUSE_AND_EXIT();
}
