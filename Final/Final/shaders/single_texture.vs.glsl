#version 330 core																	// Set OpenGL version (3.3) and profile (core).
layout (location = 0) in vec3 aPos;													// Define the input parameter (the current vertex coordinate) and its index.
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;																	// Define the output parameter (taken by the fragment shader to texture the fragment).
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;																	// Model matrix (uniform input)
uniform mat4 view;																	// View matrix (uniform input)
uniform mat4 projection;															// Projection matrix (uniform input)
uniform mat3 normalModel;															// Model matrix for normals
void main()
{
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);	// Map the input vec3 to a vec4 and set it to gl_Position. 
	TexCoord = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = vec3(normalModel * vec3(aNorm));
}