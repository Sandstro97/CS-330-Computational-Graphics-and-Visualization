#pragma once
#ifndef __EVENTS_H__
namespace events {
	/**
	 * Callback function that reacts to a
	 *	framebuffer resize event.
	 */
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
}
#endif//__EVENTS_H__