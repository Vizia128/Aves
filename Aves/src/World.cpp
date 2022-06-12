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
		window.pushCube(cube.getPoseInv());

		window.moveCamera();
		window.renderWindow();

		glfwPollEvents();
	}

} //namespace Aves