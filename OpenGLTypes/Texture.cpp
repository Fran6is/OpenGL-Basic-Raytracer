#include "Texture.h"

Texture::Texture(GLint TextureType)
{
    this->TextureType = TextureType;
    glGenTextures(1, &TextureID);
}

Texture::Texture(const FTexImage& ImageData, const FTextureParameters& Parameters, bool bShouldGenerateMipMaps) 
{
    this->TextureType = ImageData.TargetTextureType;
    glGenTextures(1, &TextureID);
    SetTexImage2D(ImageData, bShouldGenerateMipMaps);
    SetParameters(Parameters);
}

void Texture::StaticBindTexture(GLint TextureType, unsigned int TextureID, unsigned int TextureUnitToBindAt)
{
    glActiveTexture(GL_TEXTURE0 + TextureUnitToBindAt); 
    glBindTexture(TextureType, TextureID);
}

void Texture::Bind(unsigned int TextureUnit) const
{
    if( (Width + Height) == 0)
    {
        std::cerr << "Texture::Bind: seems you haven't called 'SetTexImage2D()' yet, because texture's width and height is zero \n";
    }
    
    Texture::StaticBindTexture(TextureType, TextureID, TextureUnit);
}

void Texture::UnBind() const
{
	glBindTexture(TextureType, 0);
}


void Texture::SetParameters(const FTextureParameters& Parameters) 
{
    StaticSetParameters
    (
        TextureID,
        TextureType,
        Parameters
    );
}

void Texture::StaticSetParameters(GLuint TextureID, GLint  TextureType, const FTextureParameters& Parameters)
{
    const int TotalParametersToSet = 5; 
    //Five opengl texture parameter settings for now
    /*
    Mappings:
    [ 
        0 = GL_TEXTURE_WRAP_S, 
        1 = GL_TEXTURE_WRAP_T, 
        2 = GL_TEXTURE_WRAP_R, 
        3 = GL_TEXTURE_MIN_FILTER, 
        4 = GL_TEXTURE_MAG_FILTER 
    ]
    */

    glBindTexture(TextureType, TextureID);

    std::pair<GLenum, GLint> KeyValuePair;
    for (int ParameterID = 0; ParameterID < TotalParametersToSet; ParameterID++)
    {
        if(GetParameterAsKeyValuePair(ParameterID, Parameters, KeyValuePair))
        { 
            glTexParameteri(TextureType, KeyValuePair.first, KeyValuePair.second);
        }
    }
    
    glBindTexture(TextureType, 0);
}


void Texture::SetTexImage2D(const FTexImage& ImageData, bool bShouldGenerateMipMaps) 
{
    Width  = ImageData.Width;
    Height = ImageData.Height;

    TextureType = ImageData.TargetTextureType;

    StaticSetTexImage2D
    (
        ImageData,
        TextureID,
        bShouldGenerateMipMaps
    );

}

void Texture::StaticSetTexImage2D(const FTexImage& ImageData, GLuint TextureID, bool bShouldGenerateMipMap) 
{
    glBindTexture(ImageData.TargetTextureType, TextureID);

    glTexImage2D(
    ImageData.TargetTextureType, 
    ImageData.LOD, 
    ImageData.InternalFormat, 
    ImageData.Width, 
    ImageData.Height, 
    ImageData.Border, 
    ImageData.Format, 
    ImageData.DataType, 
    ImageData.Data
    );
   
    
    if(bShouldGenerateMipMap) glGenerateMipmap(ImageData.TargetTextureType);
    glBindTexture(ImageData.TargetTextureType, 0);
}

Texture::~Texture() 
{
    glDeleteTextures(1, &TextureID);
}

bool Texture::GetParameterAsKeyValuePair(int ParameterID, const FTextureParameters& Parameters, std::pair<GLenum, GLint>& OUTKeyValuePair)
{    
    switch (ParameterID)
    {
    case 0: 
        OUTKeyValuePair.first  = GL_TEXTURE_WRAP_S; 
        OUTKeyValuePair.second = static_cast<GLint>(Parameters.WrapS);
        return Parameters.WrapS != ETextureWrapping::None;

    case 1: 
        OUTKeyValuePair.first  = GL_TEXTURE_WRAP_T; 
        OUTKeyValuePair.second = static_cast<GLint>(Parameters.WrapT);
        return Parameters.WrapT != ETextureWrapping::None;

    case 2: 
        OUTKeyValuePair.first  = GL_TEXTURE_WRAP_R; 
        OUTKeyValuePair.second = static_cast<GLint>(Parameters.WrapR);
        return Parameters.WrapR != ETextureWrapping::None;

    case 3: 
        OUTKeyValuePair.first  = GL_TEXTURE_MIN_FILTER; 
        OUTKeyValuePair.second = static_cast<GLint>(Parameters.DownSamplingFunction);
        return Parameters.DownSamplingFunction != ETextureResamplingFunction::None;

    case 4: 
        OUTKeyValuePair.first  = GL_TEXTURE_MAG_FILTER; 
        OUTKeyValuePair.second = static_cast<GLint>(Parameters.UpSamplingFunction);
        return Parameters.UpSamplingFunction != ETextureResamplingFunction::None;
    
    default: return false;
    }
}


