#pragma once
#ifndef __MODELS_H__
#define __MODELS_H__

#include <glm/glm.hpp>

#include "lights.h"

struct tex_mesh {
	unsigned int texture;
	unsigned int texture_offset;
	unsigned int VAO;
	unsigned int number_of_vertices;
	glm::mat4 model;

	float shine = 0.f;
};
typedef struct tex_mesh Model;

struct material {
	unsigned int specular_map;
	float shine = 0.f;
};
typedef struct material Material;

void models_init();

Model get_desk_model(const char* texture_path);

Model get_switch_model(const char* texture_path);

Model get_soda_model(const char* texture_path);

void draw_model(Model model, glm::mat4 projection, glm::mat4 view, RadiantLight point_light, DirectionalLight dir_light, glm::vec3 viewPos);

void draw_material_model(Model model, Material mat, glm::mat4 projection, glm::mat4 view, RadiantLight point_light, DirectionalLight dir_light, glm::vec3 viewPos);

void draw_normals(Model model, glm::mat4 projection, glm::mat4 view);
#endif//__MODELS_H__
