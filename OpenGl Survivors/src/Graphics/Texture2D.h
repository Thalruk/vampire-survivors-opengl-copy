#pragma once
#include <glad/glad.h>

class Texture2D
{
public:
    unsigned int ID;
    unsigned int Width, Height;
    unsigned int Internal_Format; 
    unsigned int Image_Format;   

    Texture2D();

    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    void Bind() const;
};