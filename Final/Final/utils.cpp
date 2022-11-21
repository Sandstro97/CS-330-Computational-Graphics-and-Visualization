#include <GLEW/glew.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utils.h"

unsigned int load_wrap_texture(const char* texture_path) {
	unsigned int texture;

	glGenTextures(1, &texture);																	// Generate texture and set texture1 to new texture's ID number
	glBindTexture(GL_TEXTURE_2D, texture);														// Bind texture1 to the context as a GL_TEXTURE_2D

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);								// Set wrapping parameters for bound texture (texture1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);								// Set wrapping parameters for bound texture (texture1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);														// Tell stb_image.h to flip image on y-axis (because of XY -> rowcol shenanigans)
	unsigned char* img_data = stbi_load(texture_path, &width, &height, &nrChannels, 0);			// Load raw image data to img_data and populate width, height and nrChannels
	if (img_data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}																							// On sucess, bind img_data to currently bound texture (texture1)
	else
	{
		std::cerr << "ERROR::TEXTURE::DATA::LOADING_FAILED" << std::endl;
	}																							// On sucess, print to stderr
	stbi_image_free(img_data);																	// Free img_data from RAM, as it has been copied to VRAM

	return texture;
}