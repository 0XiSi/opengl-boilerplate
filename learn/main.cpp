#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

void update();
static void CreateObjects();
static void CreateShaders();
void calculateFPS();
void calcAverageNormals(
	unsigned int* indices,
	unsigned int indexCount,
	GLfloat* vertices,
	unsigned int vertexCount,
	unsigned int vLength,
	unsigned int normalOffset
);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const GLfloat RED_TRIANGLE_Z = +0.0f;
const GLfloat BLUE_TRIANGLE_Z = -0.5f;

const GLuint VERTS_PER_TRI = 3;
const GLuint ATTRIB_PER_VERT = 6;
const GLuint TRI_BYTE_SIZE = VERTS_PER_TRI * ATTRIB_PER_VERT * sizeof(GLfloat);

GLuint VAO, VBO, EBO,
uniformModel,
uniformProjection,
uniformView,
uniformAmbientIntensity,
uniformAmbientColor,
uniformDirection,
uniformDiffuseIntensity,
uniformEyePosition,
uniformSpecularIntensity,
uniformShininess
;

bool isMovingRight = true;
float triOffset = 0.0f;
const float triMaxOffset = 0.6f;
const float triIncrement = 0.1f;

const float toRadians = 3.14f / 180;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

Window window(1366, 768);

std::vector<Mesh*> meshList;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

std::vector<Shader> shaderList;

static const char* vShader = "vertexShader.glsl";
static const char* fShader = "fragmentShader.glsl";

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

float lastTime_FPS = 0.0f;
int nbFrames = 0;
float fps = 0.0f;

int main() {
	if (window.Initialise() != 0) {
		return -1;
	}

	CreateObjects();
	CreateShaders();

	brickTexture = Texture("Textures/brick.png");
	dirtTexture = Texture("Textures/dirt.png");
	plainTexture = Texture("Textures/plain.png");
	brickTexture.LoadTexture();
	dirtTexture.LoadTexture();
	plainTexture.LoadTexture();

	shinyMaterial = Material(5.0f, 32);
	dullMaterial = Material(0.3f, 4);

	mainLight = DirectionalLight(
		+1.0f, +1.0f, +1.0f,
		+0.3f, +0.1f,
		+2.0f, -1.0f, -2.0f
	);


	pointLights[0] = PointLight(
		+0.0f, +0.0f, +1.0f,
		+0.1f, +0.1f,
		+4.0f, +0.0f, +0.0f,
		+0.3f, +0.2f, +0.1f
		
	);
	pointLightCount++;

	pointLights[1] = PointLight(
		+0.0f, +1.0f, +0.0f,
		+0.1f, +0.1f,
		-4.0f, +2.0f, +0.0f,
		+0.3f, +0.1f, +0.1f
		
	);
	pointLightCount++;

	spotLights[0] = SpotLight(
		+0.0f, +0.0f, +1.0f, // Color
		+0.1f, +1.0f,		 // Intensities
		+4.0f, +0.0f, +0.0f, // Position
		+0.0f, -1.0f, +0.0f, // Direction
		+1.0f, +0.0f, +0.0f, // Attenuation
		20.0f                // Edge value
	);
	spotLightCount++;

	spotLights[1] = SpotLight(
		+1.0f, +1.0f, +1.0f,   // Color
		+0.0f, +1.0f,		   // Intensities
		+2.0f, +0.9f, +0.0f,   // Position
		-1.0f, -0.5f, +0.0f,   // Direction
		+0.3f, +0.2f, +0.1f,   // Attenuation
		20.0f                  // Edge value
	);
	spotLightCount++;


	update();
	glfwTerminate();
	return 0;
}
void update() {
	uniformModel             = 0,
	uniformProjection        = 0,
	uniformView              = 0,
	uniformEyePosition       = 0;
	uniformSpecularIntensity = 0;
	uniformShininess         = 0;


	std::cout << "Uniforms:" << std::endl <<
		"| model             : " << uniformModel << std::endl <<
		"| projection        : " << uniformProjection << std::endl <<
		"| view              : " << uniformView << std::endl <<
		"| eyePosition       : " << uniformEyePosition << std::endl <<
		"| specularIntensity : " << uniformSpecularIntensity << std::endl <<
		"| shininess         : " << uniformShininess << std::endl <<
		"|" << std::endl;

	Camera camera = Camera(glm::vec3(0.0f, 0.4f, 2.5f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -12.0f, 5.0f, 0.2f);
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.getBufferWidth() / (GLfloat)window.getBufferHeight(), 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);

	glfwSwapInterval(0);
	while (!window.shouldClose()) {

		GLfloat now = static_cast<GLfloat>(glfwGetTime());
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(window.getKeys(), deltaTime);
		camera.mouseControl(window.getXChange(), window.getYChange());
		glClear(GL_DEPTH_BUFFER_BIT
			| GL_COLOR_BUFFER_BIT);

		shaderList[0].UseShader();

		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		spotLights[1].SetFlash(camera.getCameraPosition() + glm::vec3(0.0f, -0.1f, 0.0f), camera.getCameraDirecion());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		calculateFPS();
		window.swapBuffer();
	}
}
void CreateObjects() {
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	X      Y      Z				U	  V			NX	  NY    NZ
			-1.0f, -1.0f, -0.6f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			+0.0f, -1.0f, +1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			+1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			+0.0f, +1.0f, +0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3,
	};
	GLfloat floorVertices[] = {
		//	X      Y      Z				U	  V			NX	  NY    NZ
			-10.0f, +0.0f, -10.0f,		0.0f , 0.0f ,	0.0f, -1.0f, 0.0f,
			+10.0f, +0.0f, -10.0f,		10.0f, 0.0f ,	0.0f, -1.0f, 0.0f,
			-10.0f, +0.0f, +10.0f,		0.0f , 10.0f,	0.0f, -1.0f, 0.0f,
			+10.0f, +0.0f, +10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj2);

}
void CreateShaders() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);


}
void calculateFPS() {
	float currentTime = (float)glfwGetTime(); // Or GetTickCount() for Windows
	float timeDelta = currentTime - lastTime_FPS;

	nbFrames++;

	if (timeDelta >= 1.0f) {
		fps = (float)nbFrames / timeDelta;
		std::cout << "FPS: " << fps << std::endl;
		nbFrames = 0;
		lastTime_FPS = currentTime;
	}
}
void calcAverageNormals(
	unsigned int* indices,
	unsigned int indexCount,
	GLfloat* vertices,
	unsigned int vertexCount,
	unsigned int vLength,
	unsigned int normalOffset
) {
	for (size_t i = 0; i < indexCount; i += 3) {

		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;

		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}
	for (unsigned int i = 0; i < vertexCount / vLength; i++) {
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}