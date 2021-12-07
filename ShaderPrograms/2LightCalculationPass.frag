#version 330 core

layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;

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
    int   Type;
    vec3  Position;
    vec3  Direction;
    float Radius;

    vec3  Color;
    float Intensity;
    float Attenuation_Linear;
    float Attenuation_Quadratic;

    bool bCastShadow;
};

#define SHADING_DIFFUSE 10
#define SHADING_DIFFUSE_REFLECT 20
#define SHADING_DIFFUSE_REFRACT 30

struct RenderSettings
{
    int  ShadingType;
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
    const float CLIP_DISTANCE = 0.001;

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
const vec3 EnvironmentColor = vec3(0.001);
uniform   int    ITotalSceneLights  = 0;
uniform   Light  ISceneLights[TOTAL_SCENE_LIGHTS];
uniform   RenderSettings IRenderSetting = RenderSettings(SHADING_DIFFUSE, 0);

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
    out vec3 RayDirection,
    RenderSettings RenderSetting 
    );
void Reflect(
    Object SceneObjects[TOTAL_SCENE_OBJECTS], 
    int TotalSceneObjects, 
    Light SceneLights[TOTAL_SCENE_LIGHTS],
    int TotalSceneLights,
    RenderSettings RenderSetting,
    AHitResult HitResult,
    out vec3 InitialLitColor,
    out vec3 RayDirection,
    out vec3 RayPosition
);

float ShadowTrace (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection);
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

    vec3 LitColor = vec3(0);
    if( WasAHitFromPixelID(PixelID) )
    {
        AHitResult HitResult;
        HitResult.HitLocation = texture(iGPosition, TexCoord).rgb;
        HitResult.HitNormal   = normalize( texture(iGNormal, TexCoord).rgb );
        HitResult.ObjectIndex = PixelID;

        //Diffuse shading 
        LitColor = GetLitColor(
            ISceneLights,
            ITotalSceneLights,
            ISceneObjects,
            HitResult,
            RayDirection,
            IRenderSetting
        );

        //check if object is reflective ( r > 0) and or refractive (IOR > 0). The default  is -1
        if( ObjectIsReflective(ISceneObjects[PixelID], IRenderSetting.ShadingType) )
        {
            Reflect(
                ISceneObjects, 
                ITotalSceneObjects, 
                ISceneLights,
                ITotalSceneLights,
                IRenderSetting,
                HitResult,
                LitColor,
                RayDirection,
                RayOrigin
            );

        }
        else if( ObjectIsRefractive(ISceneObjects[PixelID], IRenderSetting.ShadingType) )
        {

        }

        //gl_FragColor = vec4(LitColor, 0.0);
    }
    else
    {
        LitColor = EnvironmentColor;
    }

    GPosition.rgb = LitColor;
    GPosition.a   = PixelID;
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
    out vec3 RayDirection,
    RenderSettings RenderSetting
    )
{
    //if surface is a 100 percent reflective, then there's no point calculating it's diffuse color
    //since it will just blend a 100 percent to it's environment
    if( 
          (RenderSetting.ShadingType == SHADING_DIFFUSE_REFLECT) 
        &&( RenderSetting.MaximumReflectionBounces > 0 )
        &&(SceneObjects[HitResult.ObjectIndex].Reflectivity >= 1.0)
    )
    {
        return vec3(0.0);
    }


    vec3 LitColor = vec3(0);

    for(int i = 0; i < TotalSceneLights; i++)
    {
        Light CurrentLight = SceneLights[i];

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

        if( CurrentLight.bCastShadow )
        {
            
            if( 
                ShadowTrace(
                    SceneObjects,
                    3,
                    HitResult.HitLocation + 0.001,
                    normalize(ToLightSource)
                )  
                <= length(ToLightSource)
            ) continue;

           
        }

        float Attenuation = 1;

        //Light attenuation for non-directional lights
        if( CurrentLight.Type != LIGHT_DIRECTIONAL )
        {
            float Distance    = length(ToLightSource);
            Attenuation  = Distance * Distance * CurrentLight.Attenuation_Quadratic ;
            Attenuation += Distance * CurrentLight.Attenuation_Linear;
            Attenuation += 1.0; //constant
            Attenuation  = 1.0 / Attenuation;

            //return vec3(Attenuation);
        }
        
        ToLightSource = normalize(ToLightSource);

        //Diffuse calculation
        float N_Dot_D             = dot(HitResult.HitNormal, ToLightSource);
        float DiffuseDistribution = max(N_Dot_D, 0.0) * CurrentLight.Intensity;
        
        LitColor += SceneObjects[HitResult.ObjectIndex].Color 
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

void Reflect(
    Object SceneObjects[TOTAL_SCENE_OBJECTS], 
    int TotalSceneObjects, 
    Light SceneLights[TOTAL_SCENE_LIGHTS],
    int TotalSceneLights,
    RenderSettings RenderSetting ,
    AHitResult HitResult,
    out vec3 InitialLitColor,
    out vec3 RayDirection,
    out vec3 RayPosition
    )
{
    AHitResult OldHitResult = HitResult;
    vec3 OldRay_Position    = RayPosition;
    vec3 OldRay_Direction   = RayDirection;

    int RayBounces = 0;
    while( RayBounces++ < RenderSetting.MaximumReflectionBounces )
    {
        //First make sure the previous surface hit is reflective before shooting a new ray from there
        if( SceneObjects[OldHitResult.ObjectIndex].Reflectivity > 0.0 )
        {
            vec3 NewRay_Position  = OldHitResult.HitLocation;
            vec3 NewRay_Direction = reflect(OldRay_Direction, OldHitResult.HitNormal);
            
            AHitResult NewHitResult = TraceScene(SceneObjects, TotalSceneObjects, NewRay_Position, NewRay_Direction);
            if(NewHitResult.bWasAHit)
            {

                vec3 SurfaceHit_LitColor = GetLitColor(
                    SceneLights,
                    TotalSceneLights,
                    SceneObjects,
                    NewHitResult,
                    NewRay_Direction,
                    RenderSetting
                );
                    
                    
                InitialLitColor = mix(
                    InitialLitColor, 
                    SurfaceHit_LitColor, 
                    clamp(SceneObjects[OldHitResult.ObjectIndex].Reflectivity, 0.0, 1.0)
                );
                
                OldHitResult = NewHitResult;
                OldRay_Position  = NewRay_Position;
                OldRay_Direction = NewRay_Direction;
            }
            else 
            {
                InitialLitColor = mix(
                    InitialLitColor, 
                    EnvironmentColor,//texture(CubemapTexture, NewRay_Direction).rgb,
                    clamp(SceneObjects[OldHitResult.ObjectIndex].Reflectivity, 0.0, 1.0)
                  );

                break;
            }
        }
    }
   
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

float ShadowTrace (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection)
{
    
    for(int i = 0; i < TotalObjects; i++)
    {
        AHitResult HitResult = SceneObjects[i].Type == OBJECT_SPHERE 
            ?   SphereRayIntersection(SceneObjects[i], RayPosition, RayDirection)
            :   PlaneRayIntersection( SceneObjects[i], RayPosition, RayDirection) ;
        
        if(HitResult.bWasAHit)
        {
            return HitResult.Distance;
        }
    }
    
    return 1000000;
}

bool ObjectIsReflective(Object Obj, int ShadingType)
{
    return  (Obj.Reflectivity > 0.)
         &&(ShadingType == SHADING_DIFFUSE_REFLECT );
}

bool ObjectIsRefractive(Object Obj, int ShadingType)
{
    return  Obj.IOR > 0.
          &&(ShadingType == SHADING_DIFFUSE_REFRACT );
}