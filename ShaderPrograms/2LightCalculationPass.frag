#version 330 core

//common start
uniform vec2  IResolution     = vec2(800.0, 600.0);
uniform mat3  ICameraBasis    = mat3(1.0);
uniform vec3  ICameraPosition = vec3(0.0);
uniform float ICameraFOV      = 90;

const int TOTAL_SCENE_OBJECTS = 5;
const int TOTAL_SCENE_LIGHTS  = 5;

const int NO_HIT_ID = 10000;

struct AHitResult { bool bWasAHit; vec3 HitLocation; vec3 HitNormal; int ObjectIndex; float Distance; };

#define OBJECT_PLANE  12
#define OBJECT_SPHERE 100

struct Object
{
    int   Type;
    int   ID;
    vec3  Position;
    float Scale;

    vec3  Color;
    float Diffuseness;
    float Specularity;
    float Reflectivity; //btw 0 and 1
    float IOR;
};
//common end

//Light section
#define LIGHT_DIRECTIONAL 1000
#define LIGHT_POINT       2000

struct Light
{
    int Type;
    vec3 Position;
    vec3 Direction;
    float Radius;

    vec3 Color;
    float Intensity;
    float Attenuation_Linear;
    float Attenuation_Quadratic;

    bool bCastShadow;
};

#define SHADING_DIFFUSE 10
#define SHADING_DIFFUSE_REFLECT 20
#define SHADING_DIFFUSE_REFRACT 30
#define SHADING_DIFFUSE_REFLECT_REFRACT 40

struct RenderSettings
{
    int  ShadingType;
    bool bAllowReflection;
    bool bAllowRefraction;
    int  MaximumReflectionBounces;
};
//light section end

//common begin
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
                Closest_Index = i;
            }
            
            if(CurrentHitResult.Distance < Closest_Distance)
            {
                Closest_Distance = CurrentHitResult.Distance;
                FinalHitResult   = CurrentHitResult;
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

uniform   int    ITotalSceneObjects = 0;
uniform   Object ISceneObjects[TOTAL_SCENE_OBJECTS];
//Common End

//Light section begin
uniform   int    ITotalSceneLights  = 0;
uniform   Light  ISceneLights[TOTAL_SCENE_LIGHTS];
uniform   RenderSettings IRenderSetting = RenderSettings(SHADING_DIFFUSE, false, false, 0);

uniform sampler2D iGPosition;
uniform sampler2D iGNormal;

in vec2 TexCoord;

bool WasAHitFromPixelID(int PixelID);

bool ObjectIsReflective(Object Obj, int ShadingType);
bool ObjectIsRefractive(Object Obj, int ShadingType);

//Diffuse shading - phong
vec3 GetLitColor( 
    Light SceneLights[TOTAL_SCENE_LIGHTS], 
    int TotalSceneLights, 
    Object SceneObjects[TOTAL_SCENE_OBJECTS], 
    out AHitResult HitResult, 
    out vec3 RayDirection 
    );
//light section end

void main()
{
    //Ray Construction from fragment co-ordinate
    vec2 UV = (gl_FragCoord.xy/IResolution) * 2.0 - vec2(1.0);

    vec3 RayOrigin    = ICameraPosition;
	vec3 RayDirection = vec3(UV, 1.0);
    RayDirection.x *= tan(radians(ICameraFOV/2.0)) * IResolution.x / IResolution.y;
    RayDirection.y *= tan(radians(ICameraFOV/2.0));
    RayDirection    = normalize( ICameraBasis * RayDirection );
    //

    //Get object index in alpha channel and convert to an int
    int PixelID  = int(texture(iGPosition, TexCoord).a);
    //

    if( WasAHitFromPixelID(PixelID) )
    {
        AHitResult HitResult;
        HitResult.HitLocation = texture(iGPosition, TexCoord).rgb;
        HitResult.HitNormal   = normalize( texture(iGNormal, TexCoord).rgb );
        HitResult.ObjectIndex = PixelID;

        //Diffuse shading 
        vec3 LitColor = GetLitColor(
            ISceneLights,
            ITotalSceneLights,
            ISceneObjects,
            HitResult,
            RayDirection
        );

        //check if object is reflective ( r > 0) and or refractive (IOR > 0). The default  is -1
        bool bIsReflective = (ISceneObjects[PixelID].Reflectivity > 0.) &&
                             (  
                                 IRenderSetting.ShadingType == SHADING_DIFFUSE_REFLECT 
                              || IRenderSetting.ShadingType == SHADING_DIFFUSE_REFLECT_REFRACT 
                             );
        bool bIsRefractive = (ISceneObjects[PixelID].IOR > 0.) &&
                             (  
                                 IRenderSetting.ShadingType == SHADING_DIFFUSE_REFRACT 
                              || IRenderSetting.ShadingType == SHADING_DIFFUSE_REFLECT_REFRACT 
                             );

        if( bIsReflective )
        {

        }
        else if( bIsRefractive )
        {

        }

        gl_FragColor = vec4(LitColor, 0.0);
    }
    else
    {
        gl_FragColor = vec4(0.08);
    }
}

bool WasAHitFromPixelID(int PixelID)
{
    return !(PixelID == NO_HIT_ID);
}

vec3 GetLitColor( 
    Light SceneLights[TOTAL_SCENE_LIGHTS], 
    int TotalSceneLights, 
    Object SceneObjects[TOTAL_SCENE_OBJECTS], 
    out AHitResult HitResult, 
    out vec3 RayDirection 
    )
{
    vec3 LitColor = vec3(0);
    const vec3 MinimumColorValue = vec3(0.01);

    for(int i = 0; i < TotalSceneLights; i++)
    {
        Light CurrentLight = SceneLights[i];
        CurrentLight.Color = max(CurrentLight.Color, MinimumColorValue);

        vec3 ToLightSource = vec3(0);
        
        if( CurrentLight.Type == LIGHT_POINT)
        {
            ToLightSource = CurrentLight.Position - HitResult.HitLocation;

            // float D = (ToLightSource.x * ToLightSource.x) + (ToLightSource.y * ToLightSource.y) + (ToLightSource.z * ToLightSource.z);

            // //exclude regions outside light's radius
            // if( D > (CurrentLight.Radius * CurrentLight.Radius) )
            // {
            //     continue;
            // }
             
        }
        else if( CurrentLight.Type == LIGHT_DIRECTIONAL )
        {
            ToLightSource = -CurrentLight.Direction;
        }
        else continue;

        float Attenuation = 1;

        //Light attenuation for non-directional lights
        if( CurrentLight.Type != LIGHT_DIRECTIONAL )
        {
            float Distance    = length(ToLightSource);
            Attenuation  = Distance * Distance * CurrentLight.Attenuation_Quadratic ;
            Attenuation += Distance * CurrentLight.Attenuation_Linear;
            Attenuation += 1.0; //constant
            Attenuation  = 1.0 / Attenuation;
        }
        
        ToLightSource = normalize(ToLightSource);

        //Diffuse calculation
        float N_Dot_D             = dot(HitResult.HitNormal, ToLightSource);
        float DiffuseDistribution = max(N_Dot_D, 0.0) * CurrentLight.Intensity;
        
        LitColor += max(SceneObjects[HitResult.ObjectIndex].Color, MinimumColorValue )  
                  * clamp( SceneObjects[HitResult.ObjectIndex].Diffuseness, 0.0, 1.0) //clamp btw 0 and 1
                  * (CurrentLight.Color * DiffuseDistribution * Attenuation);
        
        //Specular calculation
        if( SceneObjects[HitResult.ObjectIndex].Specularity > 0.0 )
        {
            vec3  ReflectedLightRay = reflect(-ToLightSource, HitResult.HitNormal);
            float Reflected_Dot_Eye = max( dot(-RayDirection, ReflectedLightRay), 0.0 );
            
            float SpecularDistribution = pow( Reflected_Dot_Eye, SceneObjects[HitResult.ObjectIndex].Specularity ) 
                                       * CurrentLight.Intensity;

            LitColor += SceneObjects[HitResult.ObjectIndex].Color  
                      * (CurrentLight.Color * SpecularDistribution * Attenuation );
        }        
    }
    
    LitColor += SceneObjects[HitResult.ObjectIndex].Color * vec3(0.01); //ambient factor
        
    return LitColor;
}

bool ObjectIsReflective(Object Obj, int ShadingType)
{
    return  Obj.Reflectivity > 0.
          &&(ShadingType == SHADING_DIFFUSE_REFLECT || ShadingType == SHADING_DIFFUSE_REFLECT_REFRACT );
}

bool ObjectIsRefractive(Object Obj, int ShadingType)
{
    return  Obj.IOR > 0.
          &&(ShadingType == SHADING_DIFFUSE_REFRACT || ShadingType == SHADING_DIFFUSE_REFLECT_REFRACT );
}

void Reflect(/*out vec3 LitColor, Object SceneObjects[], out HitResult HitResult, Light Scenelights*/)
{
    vec3 ReflectedColor; // initialized with lit color from previous stage

    //if(shading == diffuse_reflect || shading == diffuse_reflect_refract)

    //pass in lit color and blend it with lit color from other reflective surface

   //LitColor = ReflectedColor;
}

void Refract( /*out vec3 LitColor, Object SceneObjects[], out HitResult HitResult, Light Scenelights*/)
{
    vec3 RefractedColor; // initialized with lit color from previous stage
    //Pass in the lit color calculated from previous step and
    //blend with refract output

    //if refracted vector doesn't hit anything leave lit color as is

    //but only do this if
    //if(shading == diffuse_refract || shading == diffuse_reflect_refract)
            //calculate refraction 

    
    //LitColor = RefractedColor;
}

