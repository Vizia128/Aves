#include "PlayerInput.hpp"
#include <iostream>
#include <fstream>

namespace Aves {
	PlayerInput::PlayerInput(GLFWwindow* window)
		:PlayerWindow(window)
	{

	}

	void PlayerInput::processInput()
	{
		joysticInit();

		int length = sizeof(inMap) / sizeof(inMap[0]);
		for (int i = 0; i < length; i++)
		{
			switch (inMap[i].inputType)
			{
			case Keyboard:
				buttons[inMap[i].controlType] = glfwGetKey(PlayerWindow, inMap[i].keyIndex);

				break;

			case MouseButton:

				break;

			case MouseAxis:

				break;

			case JoyButton:
				buttons[inMap[i].controlType] = *(joysticks[inMap[i].deviceIndex].axes + inMap[i].keyIndex);
				break;

			case JoyAxis:
				axes[inMap[i].controlType] = *(joysticks[inMap[i].deviceIndex].axes + inMap[i].keyIndex);
				break;
			}
		}
	}

	void PlayerInput::joysticInit()
	{
		for (int j = 0;; j++)
		{
			joysticks[j].present = glfwJoystickPresent(j);
			if (!joysticks[j].present)
				break;

			joysticks[j].name = glfwGetJoystickName(j);

			joysticks[j].axes = glfwGetJoystickAxes(j, &joysticks[j].axesCount);

			joysticks[j].buttons = glfwGetJoystickButtons(j, &joysticks[j].buttonsCount);
		}

	}

	auto PlayerInput::jsonToCpp(nlohmann::json j, int i, const char* type)
	{
		return j["defines"][j["inputs"][i][type]];
	}

	nlohmann::json PlayerInput::getInputInit()
	{

		std::ifstream inputFile("src/Config/playerInput.json");
		nlohmann::json inputJson;
		inputFile >> inputJson;

		int length = sizeof(inMap) / sizeof(inMap[0]);
		for (int i = 0; i < length; i++)
		{
			inMap[i].controlType = jsonToCpp(inputJson, i, "controlType");
			inMap[i].inputType = jsonToCpp(inputJson, i, "inputType");
			inMap[i].keyIndex = jsonToCpp(inputJson, i, "keyIndex");
			inMap[i].inverted = jsonToCpp(inputJson, i, "inverted");
			inMap[i].curve = jsonToCpp(inputJson, i, "curve");

			for (int j = 0;; j++)
			{
				if (!joysticks[j].present)
					break;

				if (joysticks[j].name == inputJson[i]["deviceName"])
					inMap[i].deviceIndex = j;
			}
		}

		
		
		//j["name"] = ps.name;
		//j["address"] = ps.address;
		//j["age"] = ps.age;

		//std::ofstream o("src/test.json", std::ofstream::out);
		//o << std::setw(4) << j << std::endl;
		//o.close();

		//std::cout << std::setw(4) << j << std::endl;

		return inputJson;
	}
	

} //namespace Aves


//for (int j = 0;; j++)
//{
//	joysticks[j].present = glfwJoystickPresent(j);
//	if (joysticks[j].present)
//		break;
//
//	joysticks[j].name = glfwGetJoystickName(j);
//	std::cout << joysticks[j].name << " is present with jid = " << j << std::endl;
//
//	joysticks[j].axesCount;
//	joysticks[j].axes = glfwGetJoystickAxes(j, &joysticks[j].axesCount);
//	std::cout << std::endl << "AxesCount : " << joysticks[j].axesCount << "\tAxes :  ";
//	for (int i = 0; i < joysticks[j].axesCount; i++)
//	{
//		std::cout << *(joysticks[j].axes + i) << "\t";
//	}
//
//	joysticks[j].buttonsCount;
//	const unsigned char* buttons = glfwGetJoystickButtons(j, &joysticks[j].buttonsCount);
//	std::cout << std::endl << "ButtonCount : " << joysticks[j].buttonsCount << "\tButtons :  ";
//	for (int i = 0; i < joysticks[j].buttonsCount; i++)
//	{
//		std::cout << *(buttons + i) << "\t";
//	}
//}