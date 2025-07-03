#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

class Light {
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity
	);

	~Light();

protected:
	glm::vec3 color;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;
};

