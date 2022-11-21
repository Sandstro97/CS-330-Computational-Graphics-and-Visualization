#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

/**
* Forward declares all functions in "main.cpp" for unit testing
* purposes.
*/
#include "main.h"

/**
 * Contains "load_wrap_texture()"
 */
#include "utils.h"

 /**
  * Contains callbacks for GLFW events.
  */
#include "events.h"

  /**
   * Contains the "RadiantLight" class
   */
#include "lights.h"

   /**
	* Contains the "Model" class
	*/
#include "models.h"

	/**
	 * All global variables (primarily for the camera)
	 */
namespace glob {
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float cameraSpeed = 2.5f;

	bool firstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;

	//timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	bool orthographic = false;
	bool wireframe = false;
	bool zoom = false;
	int pointLightColor = 0;
}

/**
 * Declare function prototypes for GLFW callbacks that rely on global variables
 */

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

/**
 * main(int argc, char* argv[]) - Initializes the necessary libraries for ppening an OpenGL context and houses the
 *	main render loop for the created GLFW window.
 *
 *		argc - the number of arguements used to launch this program, including the binary name
 *
 *		argv - an ordered list of the arguements used to launch this program, starting with the binary name
 */
int main(int argc, char* argv[]) {
	GLFWwindow* window;	// Main render window

	/**
	* Initialize GLFW and create the main render window. Safely end execution
	* on failure.
	*/
	if ((window = create_glfw_window()) == nullptr) {
		return -1;			// Safely end execution with a bad value
	}

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization failed.\n";
		return -1;
	}

	/**
	 * Tell the context the portion of the rendering window to draw to (in this
	 * case the full window.)
	 *
	 * Also set clear color
	 */
	glViewport(0, 0, GLFW_WINDOW_WIDTH, GLFW_WINDOW_HEIGHT);	// Set viewport dimensions to top-left (0,0) through bottom-right (800,600) corners
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);						// Set clear color to black

	/**
	 * Enable depth testing and set how OpenGL responds to depth.
	 */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/**
	 * Set callback functions.
	 */
	glfwSetFramebufferSizeCallback(window, events::framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/**
	 * Tell GLFW to capture mouse.
	 */
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/**
	 * Generate shaders for light sources after OpenGL and GLFW are intitialized.
	 */
	lights_init();

	/**
	 * Generate universal (single texture, MVP) shader after OpenGL and GLFW are intialized.
	 */
	models_init();

	/**
	 * Create models
	 */
	RadiantLight light = get_point_light();
	DirectionalLight light2 = get_directional_light();

	Model desk = get_desk_model("data/wood.jpg");
	Model console = get_switch_model("data/switch.jpg");
	Model soda = get_soda_model("data/soda.jpg");

	Material console_mat;
	console_mat.specular_map = load_wrap_texture("data/switch_specular_map.jpg");
	console_mat.shine = 1.0f;

	/**
	 * Main rendering loop
	 */
	while (!glfwWindowShouldClose(window)) {
		float curr_time = glfwGetTime();
		glob::deltaTime = curr_time - glob::lastFrame;
		glob::lastFrame = curr_time;

		/**
		 * Deal with keypresses
		 */
		processInput(window);

		/**
		 * Clear color and depth buffers before rendering.
		 */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/**
		 * Create projection, view and model matrices to pass to shader.
		 */

		struct viewport_t {
			int x;
			int y;
			int width;
			int height;
		} viewport;																																// Struct instead of int [4] for field names
		glGetIntegerv(GL_VIEWPORT, (int*)&viewport);																							// Cast viewport_t to int [4] for the sake of glGet

		float aspect_ratio = (float)viewport.width / (float)viewport.height;																	// Calculate aspect ratio using viewport width and height

		glm::mat4 projection;
		if (glob::orthographic) {																												// Create projection matrix depending on value of glob::orthographic
			float ratio_size_per_depth = atan(glm::radians(glob::fov) / 2.f) * 2.f;																	// Multiply this variable by distance Z from the camera to get aspect ratio of ortho projection
			float distance = glm::length(glob::cameraFront - glob::cameraPos);																		// Calculate distance Z between camera and focal point
			float size_y = ratio_size_per_depth * distance;																							// Calculate height of projection
			float size_x = ratio_size_per_depth * distance * aspect_ratio;																			// Calculate width of projection

			projection = glm::ortho(-size_x, size_x, -size_y, size_y, 0.1f, 2.f * distance);														// Create orthographic projection matrix;
		}
		else {
			projection = glm::perspective(glm::radians(glob::fov), aspect_ratio, 0.1f, 100.f);														// Create perspective projection matrix
		}

		glm::mat4 view = glm::lookAt(glob::cameraPos, glob::cameraPos + glob::cameraFront, glob::cameraUp);										// Create view matrix									

		/**
		 * Set polygon mode depending on value of wireframe
		 */
		if (glob::wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/**
		 * Set color of point light
		 */
		switch (glob::pointLightColor) {
		case 0:
			light.color = glm::vec3(0.831f, 0.921f, 1.f);
			break;										// case 0: fluorescent light
		case 1:
			light.color = glm::vec3(1.f, 0.f, 0.f);
			break;										// case 1: red light
		case 2:
			light.color = glm::vec3(0.f, 1.f, 0.f);
			break;										// case 2: green light
		case 3:
			light.color = glm::vec3(0.f, 0.f, 1.f);
			break;										// case 3: blue light
		}

		/**
		 * Draw models
		 */
		draw_radiant_light(light, projection, view);													// Draw light source

		draw_model(desk, projection, view, light, light2, glob::cameraPos);								// Draw desk Model
		draw_material_model(console, console_mat, projection, view, light, light2, glob::cameraPos);	// Draw console Mode
		draw_model(soda, projection, view, light, light2, glob::cameraPos);								// Draw soda can Model


		glfwSwapBuffers(window);				// Swaps front and back framebuffers (output to screen)

		glfwPollEvents();						// Check to see if any events were triggered and call
													// the corresponding callback functions
	}

	/**
	 * End execution
	 */
	glfwTerminate();	// Safely terminate GLFW
	return 0;			// End execution with a good value
}

/**
 * Initialize GLFW and create render window with the newly created GL context.
 * Return nullptr on failure.
 */
GLFWwindow* create_glfw_window() {
	glfwInit();	// Initialize the GLFW library

	/**
	 * Tell GLFW to open a window with OpenGL 3.3 in the Core Profile mode
	 */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LOCAL_GL_VERSION[0]);	// Tell GLFW the major value of the OpenGL context we wish the new window to have
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LOCAL_GL_VERSION[1]);	// Tell GLFW the minor value of the OpenGL context we wish the new window to have
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		// Tell GLFW to open the new window in Core Profile mode

	/**
	 * Tell GLFW to create a window with the above configuration. If window
	 * creation fails, log to stderr and terminate.
	 */
	GLFWwindow* window = glfwCreateWindow(GLFW_WINDOW_WIDTH, GLFW_WINDOW_HEIGHT, "Main Window", NULL, NULL);	// Create GLFW window
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;		// Log failure to stderr
		glfwTerminate();												// Safely terminate GLFW
		return nullptr;														// End execution with a bad value
	}

	
	glfwMakeContextCurrent(window);

	return window;
}


/**
 * Create a shader program using the given vertex and shader GLSL sources.
 */
unsigned int gen_shader_program(const char* vertex_shader_source, const char* fragment_shader_source) {
	int success;
	char info_log[512];

	/**
	 * Create vertex shader
	 */
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);	// Create a vertex shader
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);	// Set the shader source
	glCompileShader(vertex_shader);									// Compile the shader

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);		// Check whether the shader compiled successfully
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);									// Get the info log for a failed compilation.
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;	// Log to stderr.
	}


	/**
	 * Create fragment shader
	 */
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);	// Create a fragment shader
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);	// Set the shader source
	glCompileShader(fragment_shader);									// Compile the shader

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);		// Check whether the shader compiled successfully
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);								// Get the info log for a failed compilation.
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;	// Log to stderr.
	}

	/**
	 * Combine shaders into one shader program and return the said program's ID.
	 */
	unsigned int shader_program = glCreateProgram();				// Create the shader program.
	glAttachShader(shader_program, vertex_shader);					// Attach the vertex shader.
	glAttachShader(shader_program, fragment_shader);				// Attach the fragment shader.
	glLinkProgram(shader_program);									// Link the shaders into one program.

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);		// Check for linking errors.
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);							// Get the info log for a failed compilation.
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;	// Log to stderr.
	}
	glDeleteShader(vertex_shader);									// Delete the vertex shader now that it lives on in the shader program.
	glDeleteShader(fragment_shader);								// Delete the fragment shader now that it lives on in the shader program.

	return shader_program;
}



/**
 * Credit goes to OpenGLSample
 */
 // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
 /// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	static bool p_pressed = false;
	static bool o_pressed = false;
	static bool i_pressed = false;

	using namespace glob;														// This method accesses and modifies global variables
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);									// When "ESC" is pressed, set the flag that tells main()'s render loop to exit

	if (!p_pressed && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		glob::orthographic ^= true;								// Toggle value of orthographic
		p_pressed = true;										// Set p_pressed to true
	}																			// When "P" is pressed toggle between perspective and orthographic projection
	if (p_pressed && glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
		p_pressed = false;										// Set p_pressed to false

	if (!o_pressed && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		glob::wireframe ^= true;								// Toggle value of wireframe
		o_pressed = true;										// Set o_pressed to true
	}																			// When "O" is pressed toggle wireframe mode On or Off
	if (o_pressed && glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
		o_pressed = false;										// Set o_pressed to false

	if (!i_pressed && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		glob::pointLightColor = (glob::pointLightColor + 1) % 4;	// Cycle through the directional light colors
		i_pressed = true;										// Set i_pressed to true
	}																			// When "O" is pressed toggle wireframe mode On or Off
	if (i_pressed && glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE)
		i_pressed = false;										// Set i_pressed to false

	if (!i_pressed && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		glob::pointLightColor = (glob::pointLightColor + 1) % 4;	// Cycle through the directional light colors
		i_pressed = true;									// Set i_pressed to true
	}																			// When "O" is pressed toggle wireframe mode On or Off
	if (i_pressed && glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE)
		i_pressed = false;								// Set i_pressed to false


	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		zoom = true;															// When "Shift" is held, scrolling behavior becomes zooming in and out
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE)
		zoom = false;															// When "Shift" is released, scrolling behavior returns to speeding up and down


	float speed = cameraSpeed * deltaTime;										// Adjust speed based on how many subframes have been skipped (deltaTime)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += speed * cameraFront;										// When "W" is pressed, move camera forwards, towards cameraFront
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= speed * cameraFront;										// When "S" is pressed, move camera backwards, away from cameraFront
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;	// When "A" is pressed, move camera left, perpindicular to cameraFront
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;	// When "D" is pressed, move camera right, perpindicular to cameraFront
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraUp * speed;											// When "Q" is pressed, move camera up, towards to cameraUp
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraUp * speed;											// When "E" is pressed, move camera up, away from cameraUp
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	using namespace glob;			// This method accesses and modifies global variables
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}								// Edge case for first mouse event, where there are no previous X and Y coordinates

	/**
	 * Determine change in X (xoffset) and change in Y (yoffset) of the mouse's coordinates
	 * and use those changes to calculate the new pitch and yaw of the camera.
	 */

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;	// reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;		// change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	/**
	 * Generate the new cameraFront from the new pitch and yaw.
	 *
	 * In other words, change where the camera is facing.
	 */
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	using namespace glob;					// This method access and modifies global variables
	if (zoom) {
		fov -= (float)yoffset;				// Zoom in on scroll up, out on scroll down
		if (fov < 1.0f)
			fov = 1.0f;						// Set FOV minimum
		if (fov > 45.0f)
			fov = 45.0f;					// Set FOV maximum
	}
	else {
		cameraSpeed -= (float)yoffset;		// Speed up on scroll up, down on scroll down
		if (cameraSpeed < 1.0f)
			cameraSpeed = 1.0f;			// Set speed minimum
		if (cameraSpeed > 5.0f)
			cameraSpeed = 5.0f;			// Set speed maximum
	}
}