#include "ModuleNetworkingClient.h"


bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	// - Create the remote address object
	// - Connect to the remote address
	// - Add the created socket to the managed list of sockets using addSocket()

	//Create socket
	socketMain = socket(AF_INET, SOCK_STREAM, 0);

	int result = 0;


	//Connect to the remote address

	serverAddress.sin_family = AF_INET;

	serverAddress.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr);


	//Connect to server
	result = connect(socketMain, (const sockaddr*)&serverAddress, sizeof(serverAddress));
	if (result == SOCKET_ERROR)
	{
		state = ClientState::Stopped;
	}

	addSocket(socketMain);

	// If everything was ok... change the state
	state = ClientState::Start;

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return state != ClientState::Stopped;
}

bool ModuleNetworkingClient::update()
{
	if (state == ClientState::Start)
	{
		// TODO(jesus): Send the player name to the server
		int error = send(socketMain, playerName.c_str(), (int)playerName.size() + 1, 0);
		//int error = sendto(socketMain, playerName.c_str(), playerName.size(), 0, (const sockaddr*)&serverAddress, sizeof(serverAddress));
		if (error == SOCKET_ERROR)
		{
			LOG("Error sending name");
		}
	}

	return true;
}

bool ModuleNetworkingClient::gui()
{
	if (state != ClientState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("%s connected to the server...", playerName.c_str());

		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, byte * data)
{
	state = ClientState::Stopped;
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	state = ClientState::Stopped;
}

