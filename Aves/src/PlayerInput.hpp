#pragma once

#include <GLFW/glfw3.h>
#include <klein/klein.hpp>
#include <nlohmann/json.hpp>


namespace Aves {

	enum InputTypes
	{
		Keyboard,
		MouseButton,
		MouseAxis,
		JoyButton,
		JoyAxis,

		NumInputTypes
	}inputTypes;

	enum Axes
	{
		CameraX, CameraY, CameraZ,

		CameraXY, CameraXZ, CameraYZ,

		Roll, Pitch, Yaw,

		NumAxes
	}axes;

	enum Buttons
	{
		Shoot,

		NumButtons
	}buttons;


	struct inputMap {
		int controlType;

		int deviceType;
		int deviceIndex;
		int inputType;
		int keyIndex;
		bool inverted;
		float curve;
	};

	class PlayerInput
	{
	public:
		PlayerInput() = delete;
		PlayerInput(GLFWwindow* window);
		~PlayerInput() {}

	void processInput();

	private:
		void joysticInit();
		auto jsonToCpp(nlohmann::json json, int i, const char* type);
		nlohmann::json getInputInit();

	private:
		GLFWwindow* PlayerWindow;

		struct joystick
		{
			bool present = false;
			const char* name;

			const float* axes;
			int axesCount;

			const unsigned char* buttons;
			int buttonsCount;
		} joysticks[12];

		float axes[NumAxes];
		bool buttons[NumButtons];

		inputMap inMap[NumAxes + NumButtons];
	};

} // namespace Aves
