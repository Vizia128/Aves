#pragma once
#include "Window.hpp"
#include "Cube.hpp"

namespace Aves {
	class World
	{
	public:
		World();
		~World();

		void run();

		
		bool shouldEnd() { return window.windowShouldClose(); }
	private:
		Window window;

		Cube cube;
	};
} //namespace Aves