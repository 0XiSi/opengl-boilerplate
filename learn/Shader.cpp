#include "Shader.h"



Shader::Shader() {
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
    uniformView = 0;
    uniformAmbientColor = 0;
    uniformAmbientIntensity = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {
    CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation) {
    std::string vertexString = ReadFile(vertexLocation);
    std::string fragmentString = ReadFile(fragmentLocation);
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation) {
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read %s! File doesn't exist.", fileLocation);
        return "";
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode) {
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShaderCode = ReadFile("vertexShader.glsl");
    std::string fragmentShaderCode = ReadFile("fragmentShader.glsl");

    std::cout << "Vertex shader length: " << vertexShaderCode.length() << std::endl;
    std::cout << "Fragment shader length: " << fragmentShaderCode.length() << std::endl;

    const GLchar* vertexSource = vertexShaderCode.c_str();
    const GLchar* fragmentSource = fragmentShaderCode.c_str();


    glShaderSource(vertexShaderID, 1, &vertexSource, 0);
    glShaderSource(fragmentShaderID, 1, &fragmentSource, 0);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);

    if (!logShaderError(vertexShaderID) || !logShaderError(fragmentShaderID))
        return;

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    if (!logProgramError(programID))
        return;

    glUseProgram(programID);
    shaderID = programID;
    uniformModel = glGetUniformLocation(programID, "model");
    uniformProjection = glGetUniformLocation(programID, "projection");
    uniformView = glGetUniformLocation(programID, "view");
    uniformAmbientColor = glGetUniformLocation(programID, "directionalLight.color");
    uniformAmbientIntensity = glGetUniformLocation(programID, "directionalLight.ambientIntensity");
    uniformDiffuseIntensity = glGetUniformLocation(programID, "directionalLight.diffuseIntensity");
    uniformDirection = glGetUniformLocation(programID, "directionalLight.direction");
    uniformSpecularIntensity = glGetUniformLocation(programID, "material.specularIntensity");
    uniformShininess = glGetUniformLocation(programID, "material.shininess");
    uniformEyePosition = glGetUniformLocation(programID, "eyePosition");

}

GLuint Shader::GetProjectionLocation() {
    return uniformProjection;
}

GLuint Shader::GetModelLocation() {
    return uniformModel;
}

GLuint Shader::GetViewLocation() {
    return uniformView;
}

GLuint Shader::GetAmbientColorLocation() {
    return uniformAmbientColor;
}

GLuint Shader::GetDiffuseIntensityLocation() {
    return uniformDiffuseIntensity;
}

GLuint Shader::GetDirectionLocation() {
    return uniformDirection;
}

GLuint Shader::GetSpecularIntensityLocation() {
    return uniformSpecularIntensity;
}

GLuint Shader::GetShininessLocation() {
    return uniformShininess;
}

GLuint Shader::GetEyePositionLocation() {
    return uniformEyePosition;
}

GLuint Shader::GetAmbientIntensityLocation() {
    return uniformAmbientIntensity;
}

void Shader::UseShader() {
    glUseProgram(shaderID);
}

void Shader::ClearShader() {
    if (shaderID != 0) {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }

    uniformModel = 0;
    uniformProjection = 0;
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = static_cast<GLint>(strlen(shaderCode));

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader);
}

Shader::~Shader() {
    ClearShader();
}

bool Shader::logStatus(
    GLuint objectID,
    PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
    PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
    GLenum statusType
) {
    GLint status;
    objectPropertyGetterFunc(objectID, statusType, &status);
    if (status != GL_TRUE) {
        GLint logLength;
        objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* log = new GLchar[logLength];
        getInfoLogFunc(objectID, logLength, NULL, log);
        std::cout << "Shader linking failed: " << log << std::endl;
        delete[] log;
        return false;
    }
    return true;
}
bool Shader::logShaderError(GLuint shaderID) {
    return logStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}
bool Shader::logProgramError(GLuint programID) {
    return logStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}
