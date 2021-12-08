#include "ImageLoader.h"
#include <stb_image\stb_image.h>
#include <array>
#include <cassert>

FTexImage ImageLoader::LoadImage(const std::string& ImagePath, bool bFlipVertically, unsigned char** PtrToData_OPTIONAL) //optinal parameter
{
    stbi_set_flip_vertically_on_load(bFlipVertically);
    FTexImage ReadImage; 
    int nrChannels{};
    ReadImage.Data = stbi_load(ImagePath.c_str(), &ReadImage.Width, &ReadImage.Height, &nrChannels, 0);
    

    if (nrChannels == 1)      ReadImage.Format = GL_RED;
    else if (nrChannels == 3) ReadImage.Format = GL_RGB;
    else if (nrChannels == 4) ReadImage.Format = GL_RGBA;

    if(!ReadImage.Data)
    {
        std::cerr << "std_image failed to load texture from '" << ImagePath << std::endl;
        assert(false);
    }

    if(PtrToData_OPTIONAL) *PtrToData_OPTIONAL = ReadImage.Data;

    return ReadImage;
}

void ImageLoader::LoadCubemapTextures(
        const std::array<std::string, 6>& TexturePaths, 
        std::array<FTexImage, 6>& Out_Result, bool bFlipVertically
        ) 
{
    int i = 0;
    for (auto &TexturePath : TexturePaths)
    {
        //std::cout << TexturePath << " = " << i <<"\n";
        Out_Result.at(i) = LoadImage(TexturePath, bFlipVertically);

        i++;
    }
    
}

void ImageLoader::FreeImage(unsigned char* Data) 
{
    stbi_image_free(Data);
}

