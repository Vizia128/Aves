#include "World.hpp"


namespace Aves {
	World::World()
		:window(Window()), playerInput(PlayerInput(window.window)), cube(PhxObject())
	{

	}

	World::~World()
	{

	}

	void World::run()
	{
		window.processInput();

		cube.step();
		window.pushCube(cube.getPoseInv());

		window.moveCamera();
		window.renderWindow();

		glfwPollEvents();
	}

} //namespace Aves