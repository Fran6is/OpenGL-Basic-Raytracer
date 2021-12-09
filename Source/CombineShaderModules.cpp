#include "CombineShaderModules.h"

#include <FilesCopy.h>

/********************************************************************************************************************************************
    Call this function to combine the shader modules at "ShaderPrograms/Modules" into complete shader files at "/ShaderPrograms/Source"
    whenever you update the content of those modules and you want to create new shader source files ("Light" and "Geometry") out of them
    Or you can just edit the current content of the source files as needed; there's no need creating entirely new ones each time
    
    To generate entirely new shader source files for Geometry and Light 
    from "/ShaderPrograms/Modules", delete old files ( "Light.frag" and "Geometry.frag") from "/ShaderPrograms/Source"
    and call this function to create new ones

    Alternatively, you can just delete all content from the source files ("Light.frag" and "Geometry.frag") 
    and add the tag "PASTE" to each file respectively, then call this function
    If source files are tagged "//UPDATED" on the first line, the file won't be updated
*********************************************************************************************************************************************/

void CombineShaderModules()
{
    //Geometry modules
    //if "Geometry.frag" doesn't exist at path, it will create one
    CopyPasteFileSection(
        "/ShaderPrograms/Source/Geometry.frag", 
        "/ShaderPrograms/Modules/Geometry_main.frag", 
        {"WHOLE_START", "WHOLE_END"}, 
        "PASTE",
        false
    );

    CopyPasteFileSection(
        "/ShaderPrograms/Source/Geometry.frag", 
        "/ShaderPrograms/Modules/Common_h.frag", 
        {"WHOLE_START", "WHOLE_END"}, 
        "COMMON_H",
        false
    );

    CopyPasteFileSection(
        "/ShaderPrograms/Source/Geometry.frag", 
        "/ShaderPrograms/Modules/Common_imp.frag", 
        {"WHOLE_START", "WHOLE_END"}, 
        "COMMON_IMP"
    );

    //Light modules
    //if "Light.frag" doesn't exist at path, it will create one
    CopyPasteFileSection(
        "/ShaderPrograms/Source/Light.frag", 
        "/ShaderPrograms/Modules/Light_main.frag", 
        {"WHOLE_START", "WHOLE_END"}, 
        "PASTE",
        false
    );

    CopyPasteFileSection(
        "/ShaderPrograms/Source/Light.frag", 
        "/ShaderPrograms/Modules/Common_h.frag", 
        {"WHOLE_START", "WHOLE_END"}, 
        "COMMON_H",
        false
    );

    CopyPasteFileSection(
        "/ShaderPrograms/Source/Light.frag", 
        "/ShaderPrograms/Modules/Light_h.frag", 
        {"WHOLE_START", "WHOLE_END"},
        "LIGHT_H",
        false
    );

    CopyPasteFileSection(
        "/ShaderPrograms/Source/Light.frag", 
        "/ShaderPrograms/Modules/Common_imp.frag", 
        {"WHOLE_START", "WHOLE_END"}, 
        "COMMON_IMP",
        false
    );

    CopyPasteFileSection(
        "/ShaderPrograms/Source/Light.frag", 
        "/ShaderPrograms/Modules/Light_imp.frag", 
        {"WHOLE_START", "WHOLE_END"}, 
        "LIGHT_IMP"
    );

}