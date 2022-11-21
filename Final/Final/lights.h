#pragma once
#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include <glm/glm.hpp>
#include "shader.h"

struct radiant_light_mesh {
	unsigned int VAO;
	unsigned int number_of_vertices;
	glm::mat4 model;
	glm::vec3 position;
	glm::vec3 color;

	glm::vec3 attenuation_coefficients;
};
typedef struct radiant_light_mesh RadiantLight;

struct directional_light_mesh {
	glm::vec3 direction;
	glm::vec3 color;
};
typedef struct directional_light_mesh DirectionalLight;

void lights_init();

RadiantLight get_point_light();

DirectionalLight get_directional_light();

void draw_radiant_light(RadiantLight light, glm::mat4 projection, glm::mat4 view);
#endif//__LIGHTS_H__