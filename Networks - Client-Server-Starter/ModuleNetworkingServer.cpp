#include "ModuleNetworkingServer.h"




//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::start(int port)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	// - Set address reuse
	// - Bind the socket to a local interface
	// - Enter in listen mode
	// - Add the listenSocket to the managed list of sockets using addSocket()

	bool ret = true;
	int iResult = 0;

	//Create Socket
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	//Set adress reuse
	iResult = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int));

	if (iResult == SOCKET_ERROR)
	{
		ret = false;
		ELOG("Server socket error");
	}

	//Create and adress object with any local address
	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	//Bind socket to the local address
	iResult = bind(listenSocket, (const sockaddr*)&bindAddr, sizeof(bindAddr));

	//Enter in listen mode
	iResult = listen(listenSocket, 3);


	addSocket(listenSocket);

	state = ServerState::Listening;

	return ret;
}

bool ModuleNetworkingServer::isRunning() const
{
	return state != ServerState::Stopped;
}



//////////////////////////////////////////////////////////////////////
// Module virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::update()
{
	return true;
}

bool ModuleNetworkingServer::gui()
{
	if (state != ServerState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Server Window");

		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("List of connected sockets:");

		for (auto &connectedSocket : connectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.address.sin_port));
			ImGui::Text("Player name: %s", connectedSocket.playerName.c_str());
		}

		ImGui::End();
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::isListenSocket(SOCKET socket) const
{
	return socket == listenSocket;
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	connectedSockets.push_back(connectedSocket);
}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET s, const InputMemoryStream& packet)
{
	// Set the player name of the corresponding connected socket proxy

	//NEW STUFF
	//for (auto &connectedSocket : connectedSockets)
	//{
	//	if (connectedSocket.socket == socket)
	//	{
	//		connectedSocket.playerName = (const char *)data;
	//	}
	//}

	for (auto &connectedSocket : connectedSockets)
	{
		if (connectedSocket.socket == s)
		{
			ClientMessage clientmsg;
			packet >> clientmsg;

			if (clientmsg == ClientMessage::Hello)
			{
				std::string outMessage;
				OutputMemoryStream outPacket;

				switch (clientmsg) {
				case ClientMessage::Hello:
					bool newplayer = true;
					packet >> connectedSocket.playerName;
					outPacket << ServerMessage::Welcome;
					outMessage = "Welc!";
					break;

				}

				outPacket << outMessage;


				int ret = sendPacket(outPacket, s);
				if (ret == SOCKET_ERROR)
				{
					reportError("Error Sending Welcome Packet");
				}
			}

			if (clientmsg == ClientMessage::Message)
			{
				std::string playername;

				for (int i = 0; i < connectedSockets.size(); ++i)
				{
					if (connectedSockets[i].socket == s)
					{
						playername = connectedSockets[i].playerName;
					}
				}

				std::string message;
				packet >> message;

				OutputMemoryStream outPacket;
				outPacket << ServerMessage::Message;

				std::string text = playername + ": " + message;
				outPacket << text;

				for (int i = 0; i < connectedSockets.size(); ++i)
				{
					int ret = sendPacket(outPacket, connectedSockets[i].socket);

					if (ret == SOCKET_ERROR)
					{
						reportError("Error Sending Welcom Packet");
					}
				}
			}
		}
	}

}

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	std::string playername;

	// Remove the connected socket from the list
	for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{
			playername = connectedSocket.playerName;
			connectedSockets.erase(it);
			break;
		}
	}

	OutputMemoryStream outPacket;
	outPacket << ServerMessage::UserDisc;

	std::string text = playername + " disconnected";
	outPacket << text;

	for (int i = 0; i < connectedSockets.size(); ++i)
	{
		int ret = sendPacket(outPacket, connectedSockets[i].socket);

		if (ret == SOCKET_ERROR)
		{
			reportError("Error Sending UserDisc Packet");
		}
	}
}

