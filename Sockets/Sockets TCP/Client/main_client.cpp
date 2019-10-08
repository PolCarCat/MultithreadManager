#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "127.0.0.1"

#define SERVER_PORT 8888

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

void client(const char *serverAddrStr, int port)
{
	// TODO-1: Winsock init

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NO_ERROR) {
		printWSErrorAndExit("CLIENT ERROR");

	}

	// TODO-2: Create socket (IPv4, stream, TCP)
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	// TODO-3: Create an address object with the server address

	struct sockaddr_in
	{
		short sin_family;			//Address family
		unsigned short sin_port;	//Port
		struct in_addr sin_addr;	//IP Adress
		char sin_zero[8];			// Not used
	};
	
	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;

	bindAddr.sin_port = htons(SERVER_PORT);

	const char* remoteAddrStr = SERVER_ADDRESS;
	inet_pton(AF_INET, remoteAddrStr, &bindAddr.sin_addr);


	// TODO-4: Connect to server
	iResult = connect(s, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));

	if (iResult == SOCKET_ERROR)
	{
		printWSErrorAndExit("CLIENT SOCKET ERROR");
	}


	for (int i = 0; i < 5; ++i)
	{
		// TODO-5:
		// - Send a 'ping' packet to the server
		// - Receive 'pong' packet from the server
		// - Control errors in both cases
		// - Control graceful disconnection from the server (recv receiving 0 bytes)
		const char* ping = "ping\n";

		iResult = sendto(s, ping, sizeof ping, 0, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));
	

		if (iResult == SOCKET_ERROR)
		{
			printWSErrorAndExit("CLIENT SOCKET ERROR");
		}

		char received[10];

		iResult = recv(s, (char*)received, 10, 0);


		if (iResult == SOCKET_ERROR)
		{
			printWSErrorAndExit("CLEINT SOCKET ERROR");
		}
		else
		{
			printf(received);
		}
	}

	// TODO-6: Close socket
	int closesocket(s);

	// TODO-7: Winsock shutdown

	iResult = WSACleanup();

}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
