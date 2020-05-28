#include "Application.h"

bool Application::LoadFiles()
{
	return false;
}

bool Application::SaveFiles()
{
	return false;
}

void Application::window_size_callback(GLFWwindow * window, int width, int height)
{
	if (height == 0)
		return;
	cam.SetAspectRatio((float)width / (float)height);
	glViewport(0, 0, width, height);
}

void Application::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		lightPlayer = lightPlayer ? false : true;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (showCursor)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			showCursor = false;
		}
		else
		{
			showCursor = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
	{
		if (wireframerender)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframerender = false;
		}
		else
		{
			wireframerender = true;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		if (fullsize)
		{
			glfwRestoreWindow(window);
			fullsize = false;
		}
		else
		{
			fullsize = true;
			glfwMaximizeWindow(window);
		}
	}
	if (key == GLFW_KEY_CAPS_LOCK && action == GLFW_PRESS)
	{
		if (superSpeed)
		{
			superSpeed = false;
		}
		else
		{
			superSpeed = true;
		}
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		Q_toggle = Q_toggle ? false : true;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		light = glm::vec4(cam.GetPos(), 1.0f);
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		if (lightDistance == minLightDistance)
			lightDistance = maxLightDistance;
		else
			lightDistance = minLightDistance;
	}
	//temp
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
		V_toggle = V_toggle ? false : true;

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
		key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		if (fullscreen)
		{
			glfwSetWindowMonitor(window, NULL, 200, 200, 800, 600, GLFW_DONT_CARE);
			fullscreen = false;
			glfwRestoreWindow(window);
			fullsize = false;
		}
		else
		{
			int w, h;
			int x, y;
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
			//const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, x, y, w, h, GLFW_DONT_CARE);//mode->refreshRate);
			fullscreen = true;
		}
	}
}

void Application::cursor_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (!showCursor)
		cam.RotateFromCursor(xpos, ypos, 0.05f); //sensitivity = 0.05
	else
		cam.ResetStartCursorPos(xpos, ypos);
}

void Application::mouse_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		placeGrass = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		removeGrass = true;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		placeGrassRepeat = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		removeGrassRepeat = true;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		placeGrassRepeat = false;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		removeGrassRepeat = false;
}

void Application::SetCallbackFunctions(GLFWwindow * window)
{
	Application* myPtr = this;
	glfwSetWindowUserPointer(window, myPtr);

	//Bind Resize func
	auto WindowSizeCallback = [](GLFWwindow* w, int a, int b)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->window_size_callback(w, a, b);
	};
	glfwSetWindowSizeCallback(window, WindowSizeCallback);

	//Bind Keys func
	auto KeyCallback = [](GLFWwindow* w, int a, int b, int c, int d)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->key_callback(w, a, b, c, d);
	};
	glfwSetKeyCallback(window, KeyCallback);

	//Bind Cursor func
	auto CursorCallback = [](GLFWwindow* w, double a, double b)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->cursor_callback(w, a, b);
	};
	glfwSetCursorPosCallback(window, CursorCallback);

	//Bind Mouse func
	auto MouseCallback = [](GLFWwindow* w, int a, int b, int c)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->mouse_callback(w, a, b, c);
	};
	glfwSetMouseButtonCallback(window, MouseCallback);
}


void Application::keysinput(GLFWwindow * window)
{
	if (lightPlayer)
	{
		glm::vec3 tmpLightPos = cam.GetPos();
		light = glm::vec4(tmpLightPos.x, tmpLightPos.y + 10.f, tmpLightPos.z, 1.f);
	}

	float cameraSpeed = 5.f * deltaTime; //(speed per frame) = (speed per second) * (time for frame)

	if (superSpeed)
		cameraSpeed *= 10.f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraSpeed *= 3.f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.MoveFrontToView(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.MoveFrontToView(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.MoveRight(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.MoveRight(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam.MoveUp(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)// && (int)cameraPos.y - 1 != 4 + (int)noise[1 + (int)cameraPos.z][(int)cameraPos.x])
		cam.MoveUp(-cameraSpeed);
}

Application::Application()
	: m_Window(nullptr)
{
	//Init srand
	std::srand(unsigned(std::time(0)));

	if (!OpenGLinit(m_Window))
	{
		return;
	}

	//Bind callback functions
	SetCallbackFunctions(m_Window);

	//Set Mouse mode
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Application::Run()
{
	if (m_Window == nullptr)
		return;

	float positions[] = {
		// Position             UV             Normales
		-0.5f, 0.5f, 0.5f,   0.f, 1.f,     0.f,  0.f,  1.f,  //0
		 0.5f, 0.5f, 0.5f,   1.f, 1.f,     0.f,  0.f,  1.f,  //1   FRONT
		-0.5f,-0.5f, 0.5f,   0.f, 0.f,     0.f,  0.f,  1.f,  //2
		 0.5f,-0.5f, 0.5f,   1.f, 0.f,     0.f,  0.f,  1.f,  //3

		-0.5f, 0.5f,-0.5f,   0.f, 1.f,    -1.f,  0.f,  0.f,  //4
		-0.5f, 0.5f, 0.5f,   1.f, 1.f,    -1.f,  0.f,  0.f,  //5   LEFT
		-0.5f,-0.5f,-0.5f,   0.f, 0.f,    -1.f,  0.f,  0.f,  //6
		-0.5f,-0.5f, 0.5f,   1.f, 0.f,    -1.f,  0.f,  0.f,  //7

		-0.5f, 0.5f,-0.5f,   0.f, 1.f,     0.f,  1.f,  0.f,  //8
		 0.5f, 0.5f,-0.5f,   1.f, 1.f,     0.f,  1.f,  0.f,  //9   TOP
		-0.5f, 0.5f, 0.5f,   0.f, 0.f,     0.f,  1.f,  0.f,  //10
		 0.5f, 0.5f, 0.5f,   1.f, 0.f,     0.f,  1.f,  0.f,  //11

		-0.5f,-0.5f, 0.5f,   0.f, 1.f,     0.f, -1.f,  0.f,  //12
		 0.5f,-0.5f, 0.5f,   1.f, 1.f,     0.f, -1.f,  0.f,  //13  BOTTOM
		-0.5f,-0.5f,-0.5f,   0.f, 0.f,     0.f, -1.f,  0.f,  //14
		 0.5f,-0.5f,-0.5f,   1.f, 0.f,     0.f, -1.f,  0.f,  //15

		 0.5f, 0.5f, 0.5f,   0.f, 1.f,     1.f,  0.f,  0.f,  //16
		 0.5f, 0.5f,-0.5f,   1.f, 1.f,     1.f,  0.f,  0.f,  //17  RIGHT
		 0.5f,-0.5f, 0.5f,   0.f, 0.f,     1.f,  0.f,  0.f,  //18
		 0.5f,-0.5f,-0.5f,   1.f, 0.f,     1.f,  0.f,  0.f,  //19

		 0.5f, 0.5f,-0.5f,   0.f, 1.f,     0.f,  0.f, -1.f,  //20
		-0.5f, 0.5f,-0.5f,   1.f, 1.f,     0.f,  0.f, -1.f,  //21  BACK
		 0.5f,-0.5f,-0.5f,   0.f, 0.f,     0.f,  0.f, -1.f,  //22
		-0.5f,-0.5f,-0.5f,   1.f, 0.f,     0.f,  0.f, -1.f   //23
	};

	unsigned int indices[] = {
		 0,  2,  1,//FRONT
		 1,  2,  3,

		 4,  6,  5,//LEFT
		 5,  6,  7,

		 8, 10,  9,//TOP
		 9, 10, 11,

		12, 14, 13,//BOTTOM
		13, 14, 15,

		16, 18, 17,//RIGHT
		17, 18, 19,

		20, 22, 21,//BACK
		21, 22, 23
	};

	unsigned int indicesSkybox[] = {
		 0,  1,  2, //FRONT
		 1,  3,  2,

		 4,  5,  6, //LEFT
		 5,  7,  6,

		 8,  9, 10, //TOP
		 9, 11, 10,

		12, 13, 14, //BOTTOM
		13, 15, 14,

		16, 17, 18, //RIGHT
		17, 19, 18,

		20, 21, 22, //BACK
		21, 23, 22
	};

	float positionsUI[] = {
		-0.0035f, 0.0035f,
		 0.0035f, 0.0035f,
		-0.0035f,-0.0035f,
		 0.0035f,-0.0035f
	};
	unsigned int indicesUI[] = {
		2, 1, 0,
		1, 2, 3
	};

	{ ///START SCOPE
		Renderer render;

		Texture emptyText(155, 155, 155);
		Texture skyboxTexture("res/textures/Skybox.png");

		VertexArray va;
		//VertexBuffer vb(positions, 8 * 5 * sizeof(float));//coords buffer
		//VertexBuffer vb(positions, 36 * 8 * sizeof(float));//coords buffer
		VertexBuffer vb(positions, 24 * 8 * sizeof(float));//coords buffer
		VertexBufferLayout layout;
		layout.Push<float>(3); // pos
		layout.Push<float>(2); // texture coords
		layout.Push<float>(3); // normale vectors
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 36);//index buffer object
		IndexBuffer ibSkybox(indicesSkybox, 36);//index buffer object

		VertexArray vaUI;
		VertexBuffer vbUI(positionsUI, 4 * 2 * sizeof(float));//coords buffer
		IndexBuffer ibUI(indicesUI, 6);//index buffer object
		VertexBufferLayout layoutUI;
		layoutUI.Push<float>(2); // pos
		vaUI.AddBuffer(vbUI, layoutUI);
		Shader shaderUI("res/shaders/UI.shader");
		Shader shaderLight("res/shaders/LightSrc.shader");
		Shader shaderSky("res/shaders/SkyBox.shader");

		//light = {8.f, 14.f, 14.f, 1.f};
		light = { 8.f, 214.f, 14.f, 1.f };


		modelstatic = glm::translate(model, glm::vec3(0.f, 3.f, -9.f));
		model = glm::translate(model, glm::vec3(0.f, 0.f, -6.f));

		glm::mat4 oneM = glm::mat4(1.0f);

		glm::ivec2 SEED;

		std::ifstream inp("save.txt");
		bool cameraReset = true;
		if (!inp)
		{
			SEED.x = rand();
			SEED.y = rand();
		}
		else
		{
			glm::vec3 tmp;
			glm::vec2 angl;
			inp >> SEED.x >> SEED.y >> tmp.x >> tmp.y >> tmp.z >> angl.x >> angl.y >> light.x >> light.y >> light.z;
			cameraReset = false;
			cam.SetPos(tmp);
			cam.SetPitch(angl.x);
			cam.SetYaw(angl.y);
			inp.close();
		}

		std::ifstream distancef("distance.txt");
		int distance = 30;
		if (inp)
		{
			distancef >> distance;
			distancef.close();
		}


		World world(distance, SEED);

		if (cameraReset)
		{
			std::cout << "World CREATED seed = {" << SEED.x << ", " << SEED.y << "}" << std::endl;

			cam.SetPos({ cam.GetPos().x, float(world.GetMaxHeight({ cam.GetPos().x, cam.GetPos().z })) + 0.5f, cam.GetPos().z });
		}
		else
			std::cout << "World LOADED = {" << SEED.x << ", " << SEED.y << "}" << std::endl;


		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(m_Window))
		{
			/* Render here */
			render.Clear();

			keysinput(m_Window);

			///TEMP
			if (V_toggle)
			{
				SEED.x = rand();
				SEED.y = rand();
				std::cout << "World CREATED seed = {" << SEED.x << ", " << SEED.y << "}" << std::endl;
				world.Recreate({ SEED.x, SEED.y });
				cam.SetPos(glm::vec3(8, 256, 8));
				//cam.SetPos({ cam.GetPos().x, float(world.GetMaxHeight({ cam.GetPos().x, cam.GetPos().z })) + 0.5f, cam.GetPos().z });
				V_toggle = false;
				light = { cam.GetPos().x, cam.GetPos().y + 20, cam.GetPos().z, 1 };
			}
			if (Q_toggle) // Toggle multi mining and multi placing
			{
				if (placeGrassRepeat)
					world.PlaceVoxelCamera(cam, VoxelType::GRASS);
				if (removeGrassRepeat)
					world.RemoveVoxelCamera(cam);

				placeGrass = false;
				removeGrass = false;
			}
			else
			{
				if (placeGrass)
				{
					world.PlaceVoxelCamera(cam, VoxelType::GRASS);

					placeGrass = false;
				}
				if (removeGrass)
				{
					world.RemoveVoxelCamera(cam);

					removeGrass = false;
				}
			}

			world.Draw(cam, light, lightDistance, deltaTime);


			//UI
			shaderUI.Bind();
			shaderUI.SetUniformMatrix4f("u_Proj", cam.GetUImatrix());
			render.Draw(vaUI, ibUI, shaderUI);

			//light
			shaderLight.Bind();
			shaderLight.SetUniformMatrix4f("u_Model", glm::scale(glm::translate(oneM, glm::vec3(light.x, light.y, light.z)), glm::vec3(0.1f, 0.1f, 0.1f))); //LIGHT
			shaderLight.SetUniformMatrix4f("u_ViewProj", cam.GetViewProj());
			render.Draw(va, ib, shaderLight, emptyText);
			//

			//Skybox
			shaderSky.Bind();
			shaderSky.SetUniformMatrix4f("u_Model", glm::scale(glm::translate(oneM, glm::vec3(cam.GetPos().x, cam.GetPos().y, cam.GetPos().z)), glm::vec3(900.0f, 900.0f, 900.0f)));
			shaderSky.SetUniformMatrix4f("u_ViewProj", cam.GetViewProj());
			render.Draw(va, ibSkybox, shaderSky, skyboxTexture);
			//


			/* Swap front and back buffers */
			glfwSwapBuffers(m_Window);

			/* Poll for and process events */
			glfwPollEvents();

			//DeltaTime
			float currentFrame = (float)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			//std::cout << "dt = " << deltaTime << "; FPS = " << 1.f / deltaTime << std::endl;
			lastFrame = currentFrame;
		} ///End while

		std::ofstream out("save.txt");
		if (out)
		{
			glm::vec3 tmp = cam.GetPos();
			glm::vec2 angl(cam.GetPitch(), cam.GetYaw());
			out << SEED.x << ' ' << SEED.y << ' ' << tmp.x << ' ' << tmp.y << ' ' << tmp.z << ' ' << angl.x << ' ' << angl.y << ' ' << light.x << ' ' << light.y << ' ' << light.z;
		}
	} /// END SCOPE

	//delete here
	glfwTerminate();
}
