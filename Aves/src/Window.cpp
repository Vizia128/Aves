#include "Window.hpp"
#include<iostream>

namespace Aves {

	Window::Window()
		: title("Aves"), width(1280), height(720), cellWidth(64), isFullscreen(false) 
	{
		Init();
	}

	Window::Window(const char* title, unsigned int width, unsigned int height, unsigned int cellWidth, bool isFullscreen)
		: title(title), width(width), height(height), cellWidth(cellWidth), isFullscreen(isFullscreen)
	{
		Init();
	}

	void Window::Init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


		window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (window == NULL) 
		{
			AVES_ERROR("Failed to create GLFW window");
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		gladLoadGL();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
		{
			AVES_ERROR("Failed to initialize GLAD");
		}


		Shader vertexShader;
		vertexShader.Init("C:/dev/Aves/Aves/src/Shaders/vertex.glsl", "VERTEX");
		Shader fragmentShader;
		fragmentShader.Init("C:/dev/Aves/Aves/src/Shaders/fragment.glsl", "FRAGMENT");

		renderShader.Init(vertexShader, fragmentShader);


		std::vector <float> vertices = Window::getVertices();


		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(cellWidth);


		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 8, &vertices[0], GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		renderShader.use();

		cameraID = renderShader.CreateUniformBlock("Camera", sizeof(camera));
		//UBOobj1 = ourShader.CreateUniformBlock("Object1", sizeof(object1));
	}

	void Window::renderWindow() {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//ourShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, numVertices);

		glfwSwapBuffers(window);

	}

	Window::~Window()
	{

	}

	// query GLFW whether relevant keys are pressed/released this frame and react accordingly
	void Window::processInput() {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		for (int key = 0; key < 128; key++)
		{
			if (glfwGetKey(window, key) == GLFW_PRESS)
				userKeyPress[key] = true;
			else
				userKeyPress[key] = false;
		}
	}

	void Window::moveCamera() {
		float e03 = userKeyPress[int('W')] - userKeyPress[int('S')];
		float e01 = userKeyPress[int('D')] - userKeyPress[int('A')];
		float e02 = userKeyPress[int('X')] - userKeyPress[int('Z')];
		
		float yaw = userKeyPress[int('Q')] - userKeyPress[int('E')];
		float pitch = userKeyPress[int('R')] - userKeyPress[int('F')];
		float roll = userKeyPress[int('T')] - userKeyPress[int('G')];

		float tempCube1 = userKeyPress[int('U')] - userKeyPress[int('I')];
		float tempCube2 = userKeyPress[int('J')] - userKeyPress[int('K')];

		if (e01 || e02 || e03)
		{
			kln::translator tr(0.1*sqrt(abs(e01) + abs(e02) + abs(e03)), e01, e02, e03);
			camera.pose = camera.pose * tr;
		}
		if (yaw || pitch || roll)
		{
			kln::rotor rtr(0.01*sqrt(abs(pitch)+abs(yaw)+abs(roll)), -pitch, yaw, -roll);
			camera.pose = camera.pose * rtr;
		}

		if (tempCube1 || tempCube2)
		{
			kln::translator tr(-abs(tempCube1) * 2 / 12, tempCube1, 0, 0);
			kln::rotor rtr(-abs(tempCube2) * 2 / 12, tempCube2, 0, 0);

			tcm += (tr * rtr);
			camera.tempCubeMotor = tcm.inverse();
		}

		glBindBuffer(GL_UNIFORM_BUFFER, cameraID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(camera), &camera);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		double tempTime = double(std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::steady_clock::now().time_since_epoch()).count()) / 1000;

		camera.time = float(fmod(tempTime, 12 * 3.14159265358979323846));
	}


	std::vector <float> Window::getVertices()
	{
		int numWidthSquares = width / cellWidth + (width % cellWidth != 0);
		int numHeightSquares = height / cellWidth + (height % cellWidth != 0);
		numVertices = numWidthSquares * numHeightSquares;

		std::vector <float> vertices(numVertices * 6);

		int pixels;

		for (int i = 0; i < numVertices * 6; i++) {
			switch (i % 6) {

			case 0:
				pixels = ((i / 6) % numWidthSquares) * cellWidth + cellWidth / 2;
				vertices[i] = float(pixels) / width * 2 - 1;
				break;

			case 1:
				pixels = ((i / 6) / numWidthSquares) * cellWidth + cellWidth / 2;
				vertices[i] = float(pixels) / height * 2 - 1;
				break;

			case 2:
				vertices[i] = 0.0;
				break;

			case 3:
				pixels = ((i / 6) % numWidthSquares) * cellWidth + cellWidth / 2;
				vertices[i] = float(pixels) / width;
				break;

			case 4:
				pixels = ((i / 6) / numWidthSquares) * cellWidth + cellWidth / 2;
				vertices[i] = float(pixels) / height;
				break;

			case 5:
				vertices[i] = 0.0;
				break;
			}

		}
		
		return vertices;
	}
}