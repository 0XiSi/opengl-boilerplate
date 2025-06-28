#include "Window.h"
#include <iostream>

Window::Window() :
    width(800),
    height(600),
    bufferWidth(0),
    bufferHeight(0),
    lastX(0),
    lastY(0),
    xChange(0),
    yChange(0),
    mouseFirstMoved(false),
    mainWindow(nullptr)
{
    for (size_t i = 0; i < 1024; i++) {
        keys[i] = false;
    }
}

Window::Window(GLint windowWidth, GLint windowHeight) :
    width(windowWidth),
    height(windowHeight),
    bufferWidth(0), bufferHeight(0),
    lastX(0),
    lastY(0),
    xChange(0),
    yChange(0),
    mouseFirstMoved(true),
    mainWindow(nullptr)
{
    for (size_t i = 0; i < 1024; i++) {
        keys[i] = false;
    }
}

int Window::Initialise()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Setup GLFW window properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the window
    mainWindow = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);
    if (!mainWindow)
    {
        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Get buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    createCallBacks();
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extension features
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return -1;
    }
    std::cout << "GLAD initialized successfully\n";

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // gray background

    glEnable(GL_DEPTH_TEST);

    glfwSetWindowUserPointer(mainWindow, this);


    // Set framebuffer size callback
    glfwSetFramebufferSizeCallback(mainWindow, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        });

    return 0;
}

void Window::createCallBacks() {
    glfwSetKeyCallback(mainWindow, handleKeys);
    glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
}

GLfloat Window::getYChange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key,  int code, int action, int mode) {
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        //std::cout << action;
        if (action == GLFW_PRESS) {
            theWindow->keys[key] = true;
            //printf("Pressed: %d\n", key);
        }
        else if (action == GLFW_RELEASE) {
            theWindow->keys[key] = false;
            //printf("Released: %d\n", key);
        }
    }
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos) {
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (theWindow->mouseFirstMoved) {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }
    theWindow->xChange = +xPos - theWindow->lastX;
    theWindow->yChange = -yPos + theWindow->lastY;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;

    //printf("x: %.6f, y:%.6f\n", theWindow->xChange, theWindow->yChange);
}


Window::~Window()
{
    if (mainWindow)
    {
        glfwDestroyWindow(mainWindow);
    }
    glfwTerminate();
}

