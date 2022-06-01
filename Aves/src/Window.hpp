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
		~Window();

		void Init();

		void renderWindow();

		void processInput();

		void moveCamera();
		
		bool windowShouldClose() {
			return glfwWindowShouldClose(window);
		}

	private:
		std::vector <float> getVertices();

	private: 
		const char* title;
		unsigned int width;
		unsigned int height;
		unsigned int cellWidth;
		unsigned int numVertices;
		bool isFullscreen;

		GLFWwindow* window;
		Shader renderShader;

		unsigned int VBO, VAO;

		GLuint cameraID;
		struct {
			glm::vec4 cameraPos = glm::dvec4(0.0, 1.0, 0.0, 0.0);
			glm::vec4 cameraDir = glm::vec4(0.0, 0.0, 1.0, 0.0);
			glm::vec2 windowRes = glm::vec2(1024, 640);
			float time = 0.0;
		}camera;

		struct {
			bool W = false;
			bool A = false;
			bool S = false;
			bool D = false;
		}userInput;
	};
}