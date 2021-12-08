#include "Cubemap.h"
#include <array>
#include <Texture.h>

#include <ImageLoader.h>


Cubemap::Cubemap(const std::array<std::string, 6>& TexturePaths, bool bVerticalFlip) 
    : Cubemap(
        TexturePaths, 
        FTextureParameters{ ETextureType::CubeMap,ETextureWrapping::ClampToEdge, ETextureWrapping::ClampToEdge, ETextureWrapping::ClampToEdge}, 
        bVerticalFlip
        )
{

    
}

Cubemap::Cubemap(const std::array<std::string, 6>& TexturePaths, const FTextureParameters& TextureParams, bool bVerticalFlip) 
{
    std::array<FTexImage, 6> CubemapTextures;
    ImageLoader::LoadCubemapTextures(TexturePaths, CubemapTextures, bVerticalFlip);

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

    //OpenGL cubemap 

    // GL_TEXTURE_CUBE_MAP_POSITIVE_X 	Right
    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X 	Left
    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y 	Top
    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 	Bottom
    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z 	Back
    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 	Front

    //Note:
    //In our application, remember +z is forward which will be -z in opengl coordinate system. so make sure 
    //to swap your front texture for your back texture
    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z 	front
    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 	back

    int i = 0;
    for (const auto & TexImage: CubemapTextures)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
        if(TexImage.Data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                TexImage.LOD, 
                GL_SRGB, 
                TexImage.Width, 
                TexImage.Height,
                TexImage.Border, 
                TexImage.Format, 
                TexImage.DataType, 
                TexImage.Data
            );

            Texture::StaticSetParameters(TextureID, GL_TEXTURE_CUBE_MAP, TextureParams );

            ImageLoader::FreeImage(TexImage.Data);
            
        }
        else
        {
            std::cerr << "Cubemap::Constructor:: No data found for texture at index " << i << "\n";
        }

        ++i;
    }

    //
    if( i < 5 )
    {
        std::cerr << "Cubemap::Constructor:: Cubemap faces is incomplete. Only loaded in '" << i+1 << "' textures \n";

    }

    UnBind();
}

void Cubemap::Bind(int Index) const
{
    Texture::StaticBindTexture(GL_TEXTURE_CUBE_MAP, TextureID, Index);
}

void Cubemap::UnBind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::~Cubemap() 
{
    glDeleteTextures(1, &TextureID);
}

