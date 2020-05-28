#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool OpenGLinit(GLFWwindow* &window, int width = 800, int height = 600, const char* title = "OpenGL_Application");