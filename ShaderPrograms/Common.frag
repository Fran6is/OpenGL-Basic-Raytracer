 //UPDATED       
 
        //copy the section enclosed within //BEGIN and //End tags to another file such as our geometry and light calculation shaders
        //using the function ' bool CopyPasteFileSection (args) ' defined in 'FileEditing.cpp'
        //Destination file tagged with //UPDATED on the first line will not be modified

//BEGIN
#version 330

layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;

in vec2 TexCoord;
uniform sampler2D iGPosition;
uniform sampler2D iGNormal;

uniform vec2  IResolution     = vec2(800.0, 600.0);
uniform mat3  ICameraBasis    = mat3(1.0);
uniform vec3  ICameraPosition = vec3(0.0);
uniform float ICameraFOV      = 90;

const int TOTAL_SCENE_OBJECTS = 5;
const int TOTAL_SCENE_LIGHTS  = 5;

struct AHitResult { bool bWasAHit; vec3 HitLocation; vec3 HitNormal; int ObjectIndex; float Distance; };

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

//
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

    bool bCastShadow;
};

//
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

//
AHitResult SphereRayIntersection(Object Sphere, vec3 RayPosition, vec3 RayDirection);
AHitResult PlaneRayIntersection(Object Plane, vec3 RayPosition, vec3 RayDirection);
AHitResult SphereRayIntersection(Object Sphere, vec3 RayPosition, vec3 RayDirection);

AHitResult TraceScene (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection)
{
    AHitResult FinalHitResult;
    FinalHitResult.bWasAHit = false;
    
    float Closest_Distance = -1.0;
    bool  bFirstTimeCheck = true;

    int Closest_Index = 0;

    for(int i = 0; i < TotalObjects; i++)
    {
        AHitResult CurrentHitResult = SceneObjects[i].Type == OBJECT_SPHERE 
            ?   SphereRayIntersection(SceneObjects[i], RayPosition, RayDirection)
            :   PlaneRayIntersection( SceneObjects[i], RayPosition, RayDirection) ;
        
        if(CurrentHitResult.bWasAHit)
        {
            if(bFirstTimeCheck)
            {
                Closest_Distance = CurrentHitResult.Distance;
                
                FinalHitResult  = CurrentHitResult;
                bFirstTimeCheck = false;
            }
            
            if(CurrentHitResult.Distance < Closest_Distance)
            {
                Closest_Distance = CurrentHitResult.Distance;
                FinalHitResult  = CurrentHitResult;
                Closest_Index = i;
            }
        }
    }
    
    if( Closest_Distance == -1.0 ) //no hit
    {
        FinalHitResult.bWasAHit = false;
        return FinalHitResult;
    }

    FinalHitResult.ObjectIndex = Closest_Index;
    return FinalHitResult;
}

AHitResult PlaneRayIntersection(Object Plane, vec3 RayPosition, vec3 RayDirection)
{
    float DistanceToPlane = -1.0;

	AHitResult Result;
	Result.bWasAHit = false;
    const float CLIP_DISTANCE = 1.0;

	if(DistanceToPlane >= CLIP_DISTANCE ) //Must be at least the specified distance "in front" of the camera. Not behind or too close
    {

    }

    return Result;
}

AHitResult SphereRayIntersection(Object Sphere, vec3 RayPosition, vec3 RayDirection)
{
    //Scale is the sphere radius

	float DistanceToSphere = dot(Sphere.Position - RayPosition, RayDirection);

	AHitResult HitResult;
	HitResult.bWasAHit = false;
    const float CLIP_DISTANCE = 1.0;

	if(DistanceToSphere >= CLIP_DISTANCE ) //Must be at least the specified distance "in front" of the camera. Not behind or too close
	{
		float Y = length( (RayPosition + RayDirection * DistanceToSphere) - Sphere.Position );

		if(Y <= Sphere.Scale) //Distance to ray projection end point must be inside the sphere 
		{
			float X = Sphere.Scale * Sphere.Scale - Y * Y; //this will evaluate to a negative value if we're inside the sphere
            if(X < 0.0) return HitResult; 

            X = sqrt(X);

            float T1 = DistanceToSphere - X; //first point of ray intersection with the sphere
            float T2 = DistanceToSphere + X; //second point of intersection/ray exit point

			HitResult.bWasAHit     = true;
			HitResult.HitLocation = RayPosition + RayDirection * T1;
			HitResult.HitNormal   = normalize( HitResult.HitLocation - Sphere.Position );
			HitResult.Distance    = T1;

			return HitResult;
		}

        //Distance to ray projection end point is outside the sphere
		return HitResult;
	}

	return HitResult;
}

uniform   Object ISceneObjects[TOTAL_SCENE_OBJECTS];
uniform   Light  ISceneLights[TOTAL_SCENE_LIGHTS];
uniform   RenderSettings IRenderSetting = RenderSettings(SHADING_DIFFUSE, false, false, 0);
//END