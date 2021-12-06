#pragma once

using namespace glm;

#define OBJECT_PLANE  12
#define OBJECT_SPHERE 100

struct Object
{
    int   Type = OBJECT_SPHERE;
    int   ID;
    vec3  Position = vec3(0);
    float Scale = 1;

    vec3  Color = vec3(0.1f);
    float Diffuseness  = 1;    //btw 0 and 1; will be clamped in shader
    float Specularity  = 100;  //specular distribution on surface will be raised to this value
    float Reflectivity = -1;   // (r <= 0) == 'non reflective'; (r > 0 && r <= 1) == 'reflective
    float IOR = -1;
};

#define LIGHT_DIRECTIONAL 1000
#define LIGHT_POINT       2000

struct Light
{
    int Type = LIGHT_POINT;
    int ID;
    vec3 Position  = vec3(0, 100, 0);
    vec3 Direction = vec3(1, 1, 0);
    float Radius = 2;

    vec3  Color     = vec3(0);
    float Intensity = 100;
    float Attenuation_Linear    = 0.00125f;;
    float Attenuation_Quadratic = 0.5f;

    bool bCastShadow = false;
};

#define SHADING_DIFFUSE 10
#define SHADING_DIFFUSE_REFLECT 20
#define SHADING_DIFFUSE_REFRACT 30
#define SHADING_DIFFUSE_REFLECT_REFRACT 40

struct RenderSettings
{
    int  ShadingType = SHADING_DIFFUSE;
    bool bAllowReflection = false;
    bool bAllowRefraction = false;
    int  MaximumReflectionBounces = 1;
};
