#include <glad/glad.h>
class Texture {  
public:  
    Texture();  
    Texture(const char* fileLoc); // Updated constructor to accept const char*  

    bool LoadTexture();  
    bool LoadTextureA();

    void UseTexture();  
    void ClearTexture();  

    ~Texture();  
private:  
    GLuint textureID;  
    int width, height, bitDepth;  
    const char* fileLocation; // Updated to const char*  
};