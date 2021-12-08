#pragma once
#include <vector>
#include <iostream>
#include <glad/glad.h>

struct FTexImage;
struct FTextureParameters;

class Cubemap
{
    GLuint TextureID;

public:
    Cubemap(const std::array<std::string, 6>& TexturePaths, bool bVerticalFlip);
    Cubemap(const std::array<std::string, 6>& TexturePaths, const FTextureParameters& TextureParams, bool bVerticalFlip);

    void Bind(int Index = 0) const;

    void UnBind() const;

    ~Cubemap();
};