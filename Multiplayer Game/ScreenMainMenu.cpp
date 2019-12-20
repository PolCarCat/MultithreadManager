#include "Networks.h"


static bool isValidPlayerName(const char *name)
{
	bool containsLetters = false;
	for (; *name != '\0'; ++name)
	{
		if (*name >= 'a' && *name <= 'z') containsLetters = true;
		if (*name >= 'A' && *name <= 'Z') containsLetters = true;
	}
	return containsLetters;
}

void ScreenMainMenu::gui()
{
	ImGui::Begin("Main Menu");

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

	ImGui::Spacing();

	ImGui::Text("Server");

	static int localServerPort = 8888;
	ImGui::InputInt("Server port", &localServerPort);

	if (ImGui::Button("Start server"))
	{
		App->modScreen->screenGame->isServer = true;
		App->modScreen->screenGame->serverPort = localServerPort;
		App->modScreen->swapScreensWithTransition(this, App->modScreen->screenGame);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Client");

	static char serverAddressStr[64] = "127.0.0.1";
	ImGui::InputText("Server address", serverAddressStr, sizeof(serverAddressStr));

	static int remoteServerPort = 8888;
	ImGui::InputInt("Server port", &remoteServerPort);

	static char playerNameStr[64] = "Player";
	ImGui::InputText("Player name", playerNameStr, sizeof(playerNameStr));


	const char* teams[] = { "FISH", "ROBOT" };
	static const char* teamStr = teams[0];
	static uint8 team = 0;

	const char* fishTypes[] = { "Salmon", "Pupperfish", "Tuna" };
	const char* robotTypes[] = { "Destroyer", "Submarine" };

	static const char* spaceshipTypeStr = fishTypes[0];
	static uint8 spaceshipType = 0;

	if (ImGui::BeginCombo("Team##combo", teamStr)) // The second parameter is the label previewed before opening the combo.
	{
		for (uint8 i = 0; i < IM_ARRAYSIZE(teams); i++)
		{
			bool is_selected = (teamStr == teams[i]); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(teams[i], is_selected))
			{
				teamStr = teams[i];
				team = i;
				spaceshipType = 0;
				if (team == Behaviour::Team::FISH) 	spaceshipTypeStr = fishTypes[0];
				else	spaceshipTypeStr = robotTypes[0];


			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	if (team == Behaviour::Team::FISH) {
		if (ImGui::BeginCombo("Spaceship##combo", spaceshipTypeStr)) // The second parameter is the label previewed before opening the combo.
		{
			for (uint8 i = 0; i < IM_ARRAYSIZE(fishTypes); i++)
			{
				bool is_selected = (spaceshipTypeStr == fishTypes[i]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(fishTypes[i], is_selected))
				{
					spaceshipTypeStr = fishTypes[i];
					spaceshipType = i;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
	}
	else {
		if (ImGui::BeginCombo("Spaceship##combo", spaceshipTypeStr)) // The second parameter is the label previewed before opening the combo.
		{
			for (uint8 i = 0; i < IM_ARRAYSIZE(robotTypes); i++)
			{
				bool is_selected = (spaceshipTypeStr == fishTypes[i]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(robotTypes[i], is_selected))
				{
					spaceshipTypeStr = robotTypes[i];
					spaceshipType = i;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
	}

	static bool showInvalidUserName = false;

	if (ImGui::Button("Connect to server"))
	{
		if (isValidPlayerName(playerNameStr))
		{
			showInvalidUserName = false;
			App->modScreen->screenGame->isServer = false;
			App->modScreen->screenGame->serverPort = remoteServerPort;
			App->modScreen->screenGame->serverAddress = serverAddressStr;
			App->modScreen->screenGame->playerName = playerNameStr;
			App->modScreen->screenGame->team = team;
			App->modScreen->screenGame->spaceshipType = spaceshipType;
			App->modScreen->swapScreensWithTransition(this, App->modScreen->screenGame);
		}
		else
		{
			showInvalidUserName = true;
		}
	}

	if (showInvalidUserName)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.2f, 1.0f));
		ImGui::Text("Please insert a valid player name.");
		ImGui::PopStyleColor();
	}

	ImGui::PopItemWidth();

	ImGui::End();
}
