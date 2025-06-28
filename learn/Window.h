

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
class Window {
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLint getBufferWidth() const { return bufferWidth; }
	GLint getBufferHeight() const { return bufferHeight; }
	bool shouldClose() { return glfwWindowShouldClose(mainWindow); }
	void swapBuffer() { glfwSwapBuffers(mainWindow); }
	GLFWwindow* getGLFWWindow() const { return mainWindow; }
	
	bool* getKeys() {
		return keys; 
	}
	GLfloat getXChange();
	GLfloat getYChange();

	~Window();
private:
	GLFWwindow *mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024];

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;

	void createCallBacks();
	static void handleKeys(GLFWwindow* window, int key, int action, int mode, int code);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};