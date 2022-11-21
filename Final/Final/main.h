#pragma once
#ifndef __MAIN_H___
#include <vector>
#include <GLFW/glfw3.h>

/**
 * Configuration constants
 */
const int LOCAL_GL_VERSION[2] = { 3, 3 };	// OpenGL version to use
const int GLFW_WINDOW_WIDTH = 800;			// Initial width of GLFW render window
const int GLFW_WINDOW_HEIGHT = 600;			// Initial height of GLFW render window

GLFWwindow* create_glfw_window();			// Initialize GLFW and create the main render window

unsigned int gen_shader_program(const char* vertex_shader_source, const char* fragment_shader_source);
#endif//__MAIN_H___
