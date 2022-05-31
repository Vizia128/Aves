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

	while (1)
	{

        //renderer.processInput();
        //renderer.moveCamera();
        //renderer.updateObjPos();
        window.renderWindow();

        glfwPollEvents();
	}
}