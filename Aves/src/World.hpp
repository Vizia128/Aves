#pragma once
#include "Window.hpp"
#include "PlayerInput.hpp"
#include "Ship.hpp"

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
		PlayerInput playerInput;
		PhxObject cube;
	};
} //namespace Aves