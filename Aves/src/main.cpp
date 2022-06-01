#include <iostream>
#include <spdlog/spdlog.h>

#include "Log.hpp"
#include "World.hpp"

int main()
{
	Aves::Log::Init();

	Aves::World world;
	

	while (!world.shouldEnd())
	{
		world.run();
	}
}