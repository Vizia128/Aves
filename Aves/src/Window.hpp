#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Log.hpp"
#include "Renderer.hpp"

namespace Aves {


	class Window
	{
	public:
		Window();

		Window(const char* title, unsigned int width, unsigned int height, unsigned int cellWidth, bool isFullscreen);

		void Init();

		~Window();

	private:
		std::vector <float> getVertices();

	private: 
		const char* title;
		unsigned int width;
		unsigned int height;
		unsigned int cellWidth;
		bool isFullscreen;

		GLFWwindow* window;
		Shader renderShader;

		unsigned int VBO, VAO;
	};
}