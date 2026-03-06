
#define STB_IMAGE_IMPLEMENTATION // stb_imageの実装を有効化
#include<stb_image.h>
#include<filesystem>
#include<iostream>
#include"Renderer.h"
#include <fstream> // Add this include to resolve incomplete type error for std::ifstream
#include <sstream> // Add this include to resolve incomplete type error for std::stringstream
#include"Object.h"
#include"Gltf3D.h"
#include"FBX.h"




Drawing::Renderer::Renderer()
{
	// Constructor implementation
	// Initialize any necessary resources or settings here
	
}

Drawing::Renderer::~Renderer()
{
	// Destructor implementation
	// Clean up any resources allocated in the constructor
	glfwTerminate(); // Ensure GLFW is properly terminated
}

void Drawing::Renderer::GL(int width, int height)
{
	// Initialize OpenGL context and settings here
	// This is typically done using a library like GLFW or SDL
	// For example, you might set up the viewport, enable depth testing, etc.
	InitGL();
	InitWindow(width, height); // Create a window with specified dimensions
	GlewInit(); // Initialize GLEW to manage OpenGL extensions
}

bool Drawing::Renderer::LoadShaderPrograms()
{
	shader3D = CreateShaderProgram(LoadShaderSource("vertex3D.vert"), LoadShaderSource("frag3D.frag")); // Load and compile shaders for 3D rendering
	if (shader3D == 0) {
		std::cerr << "Failed to create shader program for 3D rendering" << std::endl;
		return false; // Return false if shader program for 3D rendering creation fails
	}


	return true; // Return true if shader program creation is successful

}


void Drawing::Renderer::Run()
{
	try {
		GL(Width, Height);
		LoadShaderPrograms(); // Load and compile shader programs

		SetAspect(width, height); // Set the aspect ratio based on the loaded texture dimensions
		DrawInit(); // Initialize drawing resources (VAO, VBO, etc.)

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		glfwTerminate(); // Ensure GLFW is properly terminated on error
	}
}

void Drawing::Renderer::InitGL()
{
	// Initialize OpenGL context and settings here
// This is typically done using a library like GLFW or SDL
// For example, you might set up the viewport, enable depth testing, etc.
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Make the window not resizable
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}

void Drawing::Renderer::InitWindow(int Width, int Height)
{
	window = glfwCreateWindow(Width, Height, "Test", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window); // Make the window's context current

}

void Drawing::Renderer::GlewInit()
{
	// Initialize GLEW to manage OpenGL extensions
	glewExperimental = GL_TRUE; // Enable experimental features
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		throw std::runtime_error("Failed to initialize GLEW: ");
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, Width, Height); // Set the viewport size
}

/// <summary>
/// main loop
/// </summary>

void Drawing::Renderer::MainLoop()
{
	// Main rendering loop
	bool TitleFlag = false;
	auto Object3D = Owner->AddComponent<FBX>(Owner, this);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
	
		Object3D->Draw3D(1,5,5,0.8f,40,Object3D->center,2,4,10);

		glfwSwapBuffers(window); // Swap buffers to display the rendered frame
		glfwPollEvents(); // Poll for events (e.g., keyboard, mouse)

		if (CheckHitKey(GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true); // Close the window if the Escape key is pressed
		}
	}
}
/// <summary>
/// main loop end
/// </summary>



bool Drawing::Renderer::LoadTexture(const std::string& filePath, const std::string& filename)
{
	stbi_set_flip_vertically_on_load(true); // Flip the texture vertically to match OpenGL's coordinate system
	// Load a texture from a file and generate an OpenGL texture ID

	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
	if (!data) {
		throw std::runtime_error("Failed to load texture: " + filePath);
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	textureIDs.push_back(textureID); // Store the texture ID in the list

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//２DテクスチャをGPUメモリに転送
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data); // Free the image data after uploading to OpenGL


	return true;
}

bool Drawing::Renderer::DrawInit()
{

	unsigned int indices[6] = {
			0, 1, 2,
			1, 3, 2
	};
	if (VAO == 0) {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 頂点属性の設定
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // 位置
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // テクスチャ座標
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAO); // Bind the VAO for subsequent draw calls

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}


	return true; // Return true if initialization is successful
}


bool Drawing::Renderer::Draw3D(GLuint VAO, GLsizei indicesSize, GLuint texture, float x, float y, float z,float size,float radian, glm::vec3 center ,float cameraposx, float cameraposy, float cameraposz)
{
	glUseProgram(shader3D); // Use the shader program for rendering
	glBindVertexArray(VAO); // Bind the VAO for drawing
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x,y,z)); // Model matrix (identity for now)
	model = glm::scale(model, glm::vec3(size));
	glm::vec3 cameraPos = glm::vec3(cameraposx, cameraposy, cameraposz);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::lookAt(
		cameraPos, // Camera position
		center,   // Look at point
		upDirection // Up direction
	);
	glm::mat4 projection = glm::perspective(glm::radians(radian), (float)Width / (float)Height, 0.1f, 100.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader3D, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader3D, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader3D, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// Draw the elements using the bound VAO and texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture); // Bind the texture
	glUniform1i(glGetUniformLocation(shader3D, "texture"), 0); // Set the texture uniform to use texture unit 0
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); // Unbind the VAO after drawing
	return true; // Return true if drawing is successful
}

std::string Drawing::Renderer::LoadShaderSource(const std::string& path)
{
	// Initialize shaders here
	// This typically involves compiling vertex and fragment shaders, linking them into a program, etc.

	// Ensure vertexShaderFile is properly initialized
	std::ifstream ShaderFile(path);
	if (!ShaderFile.is_open()) {
		throw std::runtime_error("Failed to open vertex shader file: vertex_shader.glsl");
	}

	std::stringstream ShaderStream; // Ensure std::stringstream is included and initialized
	ShaderStream << ShaderFile.rdbuf(); // Read the file content into the stringstream


	// Additional shader initialization logic can be added here

	return ShaderStream.str(); // Return true if shader initialization is successful
}

GLuint Drawing::Renderer::CompileShader(GLenum type, const std::string& source)
{
	// Compile a shader of the specified type (vertex or fragment) from the given source code
	GLuint shader = glCreateShader(type);
	const char* shaderSource = source.c_str();
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);
	// Check for compilation errors
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
	}
	return shader; // Return the compiled shader ID
}

GLuint Drawing::Renderer::CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	// Create a shader program by compiling and linking vertex and fragment shaders
	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	// Create a shader program and attach the compiled shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		throw std::runtime_error("Shader program linking failed: " + std::string(infoLog));
	}
	glDeleteShader(vertexShader); // Delete the shaders after linking
	glDeleteShader(fragmentShader);
	return shaderProgram; // Return the created shader program ID
}

void Drawing::Renderer::SetAspect(int width, int height)
{
	float aspect = static_cast<float>(width) / height;
	float quadWidth = 1.0f;
	float quadHeight = 1.0f;

	vertices[0] = -quadWidth; vertices[1] = -quadHeight; vertices[2] = 0.0f; vertices[3] = 0.0f;
	vertices[4] = quadWidth; vertices[5] = -quadHeight; vertices[6] = 1.0f; vertices[7] = 0.0f;
	vertices[8] = -quadWidth; vertices[9] = quadHeight; vertices[10] = 0.0f; vertices[11] = 1.0f;
	vertices[12] = quadWidth; vertices[13] = quadHeight; vertices[14] = 1.0f; vertices[15] = 1.0f;
}

bool Drawing::Renderer::CheckHitKey(int key)
{
	// Check if a specific key is pressed
	if (glfwGetKey(window, key) == GLFW_PRESS) {
		entrytrigger = true;
		return true; // Return true if the key is pressed
	}
	return false; // Return false if the key is not pressed
}

bool Drawing::Renderer::CheckReleaseKey(int key)
{
	// Check if a specific key is pressed
	if (glfwGetKey(window, key) == GLFW_RELEASE) {
		releasetrigger = true;
		return true; // Return true if the key is pressed
	}
	return false; // Return false if the key is not pressed
}