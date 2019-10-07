#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

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

void server(int port)
{
	// TODO-1: Winsock init
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		return;

	// TODO-2: Create socket (IPv4, datagrams, UDP
	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
		return;

	// TODO-3: Force address reuse
	int enable = 1;
	int res = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	//if (res == SOCKET_ERROR) { // Log and handle error }

	// TODO-4: Bind to a local address
	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET; // IPv4
	bindAddr.sin_port = htons(port); // Port
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY; // Any local IP address

	res = bind(s, (const struct sockaddr *)&bindAddr, sizeof(bindAddr));
	//if (res == SOCKET_ERROR) { // Log and handle error }

	while (true)
	{
		// TODO-5:
		// - Receive 'ping' packet from a remote host
		char* ping = "";
		struct sockaddr_in remoteAddrClient;
		int sizeRemoteServer = sizeof(remoteAddrClient);
		int msg_in = recvfrom(s, ping, sizeof(ping), 0, (sockaddr*)&remoteAddrClient, &sizeRemoteServer);

		// - Answer with a 'pong' packet
		if (ping != "")
		{
			std::cout << ping << std::endl;
			const char* pong = "pong";
			int msg_out = sendto(s, pong, sizeof(pong), 0, (sockaddr*)&bindAddr, sizeof(bindAddr));
		}

		// - Control errors in both cases
	}

	// TODO-6: Close socket
	int closesocket(SOCKET s);

	// TODO-7: Winsock shutdown
	iResult = WSACleanup();
}

int main(int argc, char **argv)
{
	server(SERVER_PORT);

	PAUSE_AND_EXIT();
}
