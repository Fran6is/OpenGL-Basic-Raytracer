#pragma once

#include <glad/glad.h>
#include "TextureParameters.h"
#include <iostream>



struct FImageLoader
{
    static FTexImage LoadImage(const std::string& ImagePath, unsigned char** PtrToData_OPTIONAL = nullptr);
    static void FreeImage(unsigned char* Data);
};

class Texture
{
private:
	unsigned int TextureID{};
    GLint TextureType = GL_TEXTURE_2D;
	int Width{};
	int Height{};
    
    static bool GetParameterAsKeyValuePair(int SettingID, const FTextureParameters& Parameters, std::pair<GLenum, GLint>& OUTKeyValuePair);

public:
	explicit Texture(GLint TextureType);
    explicit Texture(const FTexImage& ImageData, const FTextureParameters& Parameters, bool bShouldGenerateMipMaps = false);

	void Bind(unsigned int TextureUnit = 0) const;
	void UnBind() const;
	~Texture();

	inline const unsigned int& GetWidth()  const {return  Width; }
	inline const unsigned int& GetHeight() const { return Height; }
    inline const unsigned int& GetTextureID() const { return TextureID; }
    inline const int& GetTextureType() const { return TextureType; }

    void SetParameters(const FTextureParameters& Parameters) ;
    void SetTexImage2D(const FTexImage& ImageData, bool bShouldGenerateMipMaps = false);

    //static versions

    //Texture parameters with a value of 'EnumPropertyName::None' won't be set
    static void StaticSetParameters(GLuint TextureID, GLint  TextureType, const FTextureParameters& Parameters);

    static void StaticSetTexImage2D(const FTexImage& ImageData, GLuint TextureID, bool bShouldGenerateMipMap);

    static void StaticBindTexture( GLint TextureType, unsigned int TextureID, unsigned int TextureUnitToBindAt = 0 );
};