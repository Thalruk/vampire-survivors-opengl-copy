#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "../../Vendors/stb_image.h" 

std::map<std::string, Texture2D> ResourceManager::Textures;

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    Texture2D texture;

    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    if (data)
    {
        texture.Generate(width, height, data);
    }
    else
    {
        std::cout << "Failed to load texture: " << file << std::endl;
    }

    stbi_image_free(data);

    return texture;
}