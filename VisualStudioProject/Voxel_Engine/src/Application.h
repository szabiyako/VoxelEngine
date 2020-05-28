#pragma once

#include "opengl/OpenGLinit.h"

#include <iostream>
#include <fstream>
#include <string>

//for GLCall

#include "opengl/Renderer.h"
#include "opengl/VertexBuffer.h"
#include "opengl/VertexBufferLayout.h"
#include "opengl/IndexBuffer.h"
#include "opengl/VertexArray.h"
#include "opengl/Shader.h"
#include "opengl/Texture.h"
#include "Camera.h"

#include "glm/glm.hpp"

//rotation
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//perlin noise
#include <glm/gtc/noise.hpp> 

#include "PerlinNoise.h"
#include "Chunk.h"
#include "World.h"

//random
#include <ctime>

class Application
{
	GLFWwindow* m_Window;

	Camera cam;// (glm::vec3(7.5f, 154.f, 7.5f));

	float maxLightDistance = 800.f;
	float minLightDistance = 200.f;
	float lightDistance = maxLightDistance;

	glm::vec4 light;

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 modelstatic = glm::mat4(1.0f);
	
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame


	bool showCursor = false;

	//keys
	bool fullsize = false;
	bool fullscreen = false;
	bool wireframerender = false;
	bool superSpeed = false;
	bool Q_toggle = false;

	bool V_toggle = false;

	bool lightPlayer = false;
	//Mouse
	bool placeGrass = false;
	bool removeGrass = false;

	bool placeGrassRepeat = false;
	bool removeGrassRepeat = false;


	//Files
	bool LoadFiles();

	bool SaveFiles();

	//resize
	void window_size_callback(GLFWwindow* window, int width, int height);
	//keys
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	//Mouse pos
	void cursor_callback(GLFWwindow* window, double xpos, double ypos);
	//Mouse buttons
	void mouse_callback(GLFWwindow* window, int button, int action, int mods);

	void SetCallbackFunctions(GLFWwindow * window);

	//KeysTemp
	//manualControl
	void keysinput(GLFWwindow* window);
public:
	Application();
	void Run();
};

