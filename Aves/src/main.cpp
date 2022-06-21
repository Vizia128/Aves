#include <iostream>
#include <istream>

#include <spdlog/spdlog.h>


#include "Log.hpp"
#include "World.hpp"



#include <stdlib.h>
#include <windows.h>


int main()
{
	Aves::Log::Init();

	Aves::World world;




	while (!world.shouldEnd())
	{
		world.run();
	}

}