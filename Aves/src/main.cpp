#include <iostream>
#include <spdlog/spdlog.h>

#include "Log.hpp"
#include "World.hpp"
#include "Window.hpp"

int main()
{
	Aves::Log::Init();

	//Aves::World world;
	Aves::Window window;

	while (!window.windowShouldClose())
	{

        window.processInput();
        window.moveCamera();
        window.renderWindow();

        glfwPollEvents();
	}
}