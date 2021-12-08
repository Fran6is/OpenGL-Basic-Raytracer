#pragma once
#include <iostream>
#include "TextureParameters.h"

struct ImageLoader
{
    static FTexImage LoadImage(const std::string& ImagePath, bool bFlipVertically = true, unsigned char** PtrToData_OPTIONAL = nullptr);

    //Read textures will be in the order the texture paths are in.
    //It should be Right = 0, Left = 1, Top = 2, Bottom = 3, Back = 4, Front = 5 array ( 'TexturePaths' ) indices; if 'Out_Result' is directly sent 'Cubemap' class constructor
    static void LoadCubemapTextures( 
        const std::array<std::string, 6>& TexturePaths, 
        std::array<FTexImage, 6>& Out_Result,
        bool bFlipVertically
    );

    static void FreeImage(unsigned char* Data);
};