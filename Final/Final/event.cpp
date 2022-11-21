#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "events.h"

namespace events {
	/**
	 * Callback to handle framebuffer resize events.
	 */
	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);	//set viewport to the full width and height of the new framebuffer
	}
}