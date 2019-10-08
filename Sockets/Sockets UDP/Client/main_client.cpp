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
	if (iResult != NO_ERROR)
		printWSErrorAndExit("ERROR STARTING CLIENT SOCKET");

	// TODO-2: Create socket (IPv4, datagrams, UDP)
	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
		printWSErrorAndExit("ERROR CREATING CLIENT SOCKET");

	// TODO-3: Create an address object with the server address
	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET; // IPv4
	remoteAddr.sin_port = htons(port); // Port
	const char *remoteAddrStr = serverAddrStr; // Not so remote… :-P
	inet_pton(AF_INET, remoteAddrStr, &remoteAddr.sin_addr);

	bool wait = false;

	// Client string
	std::string pingString("Ping");

	// Input buffer
	const int inBufferLen = 1300;
	char inBuffer[inBufferLen];

	// From address (server)
	struct sockaddr serverAddr;
	int serverAddrLen = sizeof(serverAddr);

	for (int i = 5; i > 0; --i)
	{
		// TODO-4:
		// - Send a 'ping' packet to the server
		int msg_out = sendto(s, pingString.c_str(), (int)pingString.size() + 1, 0, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
		if (msg_out != SOCKET_ERROR)
			wait = true;
		else
			printWSErrorAndExit("ERROR sending message");

		// - Receive 'pong' packet from the server
		while (wait)
		{
			int msg_in = recvfrom(s, inBuffer, inBufferLen, 0, (sockaddr*)&serverAddr, &serverAddrLen);

			if (msg_in != SOCKET_ERROR)
			{
				std::cout << "Client: " << inBuffer << std::endl;
				wait = false;

				Sleep(1000);
			}
		}

		// - Control errors in both cases
	}

	// TODO-5: Close socket
	int closesocket(SOCKET s);

	// TODO-6: Winsock shutdown
	iResult = WSACleanup();
}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
