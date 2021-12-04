#pragma once

using namespace glm;

//BEGIN
#define OBJECT_PLANE  12
#define OBJECT_SPHERE 100

struct Object
{
    int   Type;
    int   ID;
    vec3  Position;
    vec3  Rotation;
    float Scale;

    vec3  Color;
    float Specularity;
    float Reflectivity; //btw 0 and 1
    float IOR;
};

#define LIGHT_DIRECTIONAL 1000
#define LIGHT_POINT       2000

struct Light
{
    int Type;
    vec3 Position;

    vec3 Color;
    float Intensity;
    float Attenuation_Linear;
    float Attenuation_Quadratic;

    bool bCastShadow = false;
};

#define SHADING_DIFFUSE 10
#define SHADING_DIFFUSE_REFLECTION 20
#define SHADING_DIFFUSE_REFLECT_REFRACT 30

struct RenderSettings
{
    int  ShadingType;
    bool bAllowReflection;
    bool bAllowRefraction;
    int  MaximumReflectionBounces;
};
//END