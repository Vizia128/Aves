#include "World.hpp"


namespace Aves {
	World::World()
	{

	}

	World::~World()
	{

	}

	void World::run()
	{
		window.processInput();
		window.moveCamera();
		window.renderWindow();

		glfwPollEvents();
	}

}