#version 330 core
layout (location = 1) in vec3 aPos;
layout (location = 2) in vec3 aVertexColor;

out vec4 VertexColor;

uniform mat4 model;																	// Model matrix (uniform input)
uniform mat4 view;																	// View matrix (uniform input)
uniform mat4 projection;															// Projection matrix (uniform input)
void main()
{
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);	// Map the input vec3 to a vec4 and set it to gl_Position. 
	VertexColor = vec4(aVertexColor, 1.0);
}