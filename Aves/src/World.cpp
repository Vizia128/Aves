#include "World.hpp"


namespace Aves {
	World::World()
		:cube(Cube())
	{

	}

	World::~World()
	{

	}

	void World::run()
	{
		window.processInput();

		cube.step();

		window.moveCamera();
		window.renderWindow();

		glfwPollEvents();
	}

}