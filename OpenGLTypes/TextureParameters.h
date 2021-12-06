#pragma once

#include <glad/glad.h>

//more info at https://docs.gl/gl3/glTexImage2D
struct FTexImage
{
    GLint TargetTextureType = GL_TEXTURE_2D;
    GLint LOD = 0;
    GLint InternalFormat = GL_RGBA8;
    int Width {512};
	int Height{512};
    GLint Border = 0; //This value must be 0. https://docs.gl/gl3/glTexImage2D
    GLint Format = GL_RGB;
    GLenum DataType = GL_UNSIGNED_BYTE;
    unsigned char* Data = nullptr;

    static void ResetToDefaults(FTexImage& ImageData)
    {
        ImageData.InternalFormat = GL_RGBA8;
        ImageData.Height = ImageData.Width = 512;
        ImageData.Format = GL_RGB;
        ImageData.Data = nullptr;
        ImageData.TargetTextureType = GL_TEXTURE_2D;
        
    }
};

//Some texture properties in opengl using 'glTexParameter'
//Checkout https://docs.gl/gl3/glTexParameter and https://learnopengl.com/Getting-started/Textures

enum class ETextureType: GLenum
{
    None,
    Texture2D = GL_TEXTURE_2D,
    CubeMap   = GL_TEXTURE_CUBE_MAP
};
enum class ETextureWrapping: GLint
{
    None,
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE
};

enum class ETextureResamplingFunction: GLint
{
    None,
    Linear = GL_LINEAR,
    NearestNeighbor = GL_NEAREST
};

struct FTextureParameters
{
    ETextureType TextureType = ETextureType::Texture2D;
    ETextureWrapping WrapS  = ETextureWrapping::None;
    ETextureWrapping WrapT  = ETextureWrapping::None;
    
    ETextureResamplingFunction UpSamplingFunction   = ETextureResamplingFunction::Linear;
    ETextureResamplingFunction DownSamplingFunction = ETextureResamplingFunction::Linear;
};