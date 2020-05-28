#include "OpenGLinit.h"

//Settings of Init
const bool msaaEnable = true;
const int msaaSamples = 4;

//OpenGL settings setup
static void SetParameters()
{
	//For transpanent
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); //for skybox (means depth less or equal)
	glEnable(GL_CULL_FACE);
}

bool OpenGLinit(GLFWwindow* &window, int width /*= 800*/, int height /*= 600*/, const char* title /*= "OpenGL_Application"*/)
{
	/* Initialize the library */
	if (!glfwInit())
	{
		std::cout << "glfwInit() error!" << std::endl;
		return false;
	}

	//Set Depth buffer bits size
	//glfwWindowHint(GLFW_DEPTH_BITS, 32);

	//Set GLFW context version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//For msaa
	if (msaaEnable)
	{
		glfwWindowHint(GLFW_SAMPLES, msaaSamples);   // default is a 4
		glEnable(GL_MULTISAMPLE);		   // thats msaa hides light bug lol glfwWindowHint(GLFW_SAMPLES, 64);
	}

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return false;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//disable 60fps lock or set vsync// 0 - max, 1 - 60fps, 2 - 30 - fps, etc
	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Glew doesn`t work!" << std::endl;
		return false;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	//TEMP FOR TESTING
	glfwSetWindowPos(window, 800, 400);

	//Additional
	SetParameters();
	//

	return true;
}