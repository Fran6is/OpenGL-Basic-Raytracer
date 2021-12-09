WHOLE_START
#version 330 core

layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;

uniform vec2  IResolution     = vec2(800.0, 600.0);
uniform mat3  ICameraBasis    = mat3(1.0);
uniform vec3  ICameraPosition = vec3(0.0);
uniform float ICameraFOV      = 90;

uniform sampler2D   iGPosition;
uniform sampler2D   iGNormal;

struct AHitResult { bool bWasAHit; vec3 HitLocation; vec3 HitNormal; int ObjectIndex; float Distance;  };

const float TinyOffset = 0.01; //keep this distance from an already hit surface if tracing another ray from there. Used by shadow and plane-ray ray cast

#define OBJECT_PLANE  12
#define OBJECT_SPHERE 100

struct Object
{
    int   Type;
    int   ID;
    vec3  Position;
    float Scale;
    mat3  Basis; 

    vec3  Color;
    float Diffuseness;
    float Specularity;
    float Reflectivity; //btw 0 and 1
};

const int TOTAL_SCENE_OBJECTS = 5;
const int TOTAL_SCENE_LIGHTS  = 5;
const int NO_HIT_ID           = 10000;

uniform   int    ITotalSceneObjects = 0;
uniform   Object ISceneObjects[TOTAL_SCENE_OBJECTS];

AHitResult TraceScene (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection);
AHitResult SphereRayIntersection(Object Sphere, vec3 RayPosition, vec3 RayDirection);
AHitResult PlaneRayIntersection(Object Plane, vec3 RayPosition, vec3 RayDirection);
AHitResult SphereRayIntersection(Object Sphere, vec3 RayPosition, vec3 RayDirection);

WHOLE_END