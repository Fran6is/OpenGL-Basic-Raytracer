#version 330

//COMMON_START(Geometry and Light)
layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;

uniform vec2  IResolution     = vec2(800.0, 600.0);
uniform mat3  ICameraBasis    = mat3(1.0);
uniform vec3  ICameraPosition = vec3(0.0);
uniform float ICameraFOV      = 90;

struct AHitResult { bool bWasAHit; vec3 HitLocation; vec3 HitNormal; int ObjectIndex; float Distance; };

const float TinyOffset = 0.001; //keep this distance from an already hit surface if tracing another ray from there. Used by shadow and plane-ray ray cast

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
//COMMON_END(Geometry and Light)

//LIGHT_START
#define LIGHT_DIRECTIONAL 1000
#define LIGHT_POINT       2000

struct Light
{
    int   Type;
    vec3  Position;
    vec3  Direction;

    vec3  Color;
    vec3  Ambient;
    float Intensity;
    float Attenuation_Linear;
    float Attenuation_Quadratic;

    bool bCastShadow;
};

#define SHADING_DIFFUSE 10
#define SHADING_DIFFUSE_REFLECT 20

struct RenderSettings
{
    int  ShadingType;
    int  MaximumReflectionBounces;
};

uniform   int    ITotalSceneLights  = 0;
uniform   Light  ISceneLights[TOTAL_SCENE_LIGHTS];
uniform   RenderSettings IRenderSetting = RenderSettings(SHADING_DIFFUSE, 0);

uniform sampler2D   iGPosition;
uniform sampler2D   iGNormal;
uniform samplerCube iCubemap;

in vec2 TexCoord;

bool WasAHitFromPixelID(int PixelID);

//Diffuse shading - phong
vec3 GetLitColor( 
    Light SceneLights[TOTAL_SCENE_LIGHTS], 
    int TotalSceneLights, 
    Object SceneObjects[TOTAL_SCENE_OBJECTS], 
    int TotalSceneObjects,
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
    out vec3 InitialLitColor,
    out AHitResult OldHitResult,
    out vec3 OldRay_Direction,
    out vec3 OldRay_Position
);

float ShadowTrace (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection);
//LIGHT_END

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

    //TODO:
    //Image based lighting with cube maps

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
            ITotalSceneObjects,
            HitResult,
            RayDirection,
            IRenderSetting
        );

        //check if object is reflective ( r > 0) and or refractive (IOR > 0). The default  is -1
        if( (ISceneObjects[HitResult.ObjectIndex].Reflectivity > 0.) && (IRenderSetting.ShadingType == SHADING_DIFFUSE_REFLECT) )
        {
            Reflect(
                ISceneObjects, 
                ITotalSceneObjects, 
                ISceneLights,
                ITotalSceneLights,
                IRenderSetting,
                LitColor,
                HitResult,
                RayDirection,
                RayOrigin
            );

        }
    }
    else 
    {

        LitColor = texture(iCubemap, RayDirection).rgb;

    }

    GPosition.rgb =  LitColor;
    GPosition.a   = PixelID;
}

//COMMON_START(Geometry and Light)
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
    //Plane line intersection equation
    //Review at https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
    
	AHitResult Result;
	Result.bWasAHit = false;
    vec3 PlaneNormal = Plane.Basis[1];

    // assuming vectors are all normalized
    float Denominator = dot( PlaneNormal, RayDirection); 

    //if the dot product btw the plane normal and ray direction is equal to zero, then the ray won't intercept the plane
    //because they are perpendicular. Or some value close to zero due to floating point precision

    if( abs(Denominator) > 0.00006 ) 
    { 
        vec3 RayToPlane = Plane.Position - RayPosition;

        float T = dot(RayToPlane, PlaneNormal) / Denominator;

        //if 'RayToPlane' dot 'PlaneNormal' = 0, then the ray is exactly on the plane
        //this might affect reflection traces since they start exactly on the plane
        //Solution is to add a small offset in the direction of the ray's normal when sending final hit location

        if(T >= 0.)
        {
            vec3 SupposedPointOnPlane = RayPosition + RayDirection * T;

            vec3 DistanceToPlane =  abs( transpose(Plane.Basis) * (SupposedPointOnPlane - Plane.Position) ) - vec3(Plane.Scale);            
            
            DistanceToPlane = max(DistanceToPlane, vec3(0.0));

            if( length(DistanceToPlane) <= TinyOffset )
            {
                Result.bWasAHit    = true;
                Result.HitLocation = SupposedPointOnPlane + PlaneNormal * TinyOffset;
                Result.HitNormal   = PlaneNormal;
                Result.Distance    = T;
            }

        }
    } 
     
    return Result;
}

AHitResult SphereRayIntersection(Object Sphere, vec3 RayPosition, vec3 RayDirection)
{
    //Scale is the sphere radius

	float DistanceToSphere = dot(Sphere.Position - RayPosition, RayDirection);

	AHitResult HitResult;
	HitResult.bWasAHit = false;

	if(DistanceToSphere >= 1.0 ) //Must be at least the specified distance "in front" of the camera. Not behind or too close
	{
		float Y = length( (RayPosition + RayDirection * DistanceToSphere) - Sphere.Position );

		if(Y <= Sphere.Scale) //Distance to ray projection end point must be inside the sphere 
		{
			float X = sqrt( Sphere.Scale * Sphere.Scale - Y * Y ); 

            //T1 will be negative if ray is inside the sphere because then X will be greater than 'DistanceToSphere'
            //T2 at this point will be outside the sphere rather than on it (on the exit side) because 
            //we'll be adding to 'DistanceToSphere'

            //If ray position is exactly on the surface, then T1 will equal 0 ; and T2 is the distance to the exit location (as usual)
            //T1 = T2 when there's only one entry and exit
            
            float T1 = DistanceToSphere - X; //Entry
            float T2 = DistanceToSphere + X; //Exit

			HitResult.bWasAHit    = true;
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
//COMMON_END(Geometry and Light)


//LIGHT_START
bool WasAHitFromPixelID(int PixelID)
{
    return !(PixelID == NO_HIT_ID);
}

vec3 GetLitColor( 
    Light SceneLights[TOTAL_SCENE_LIGHTS], 
    int TotalSceneLights, 
    Object SceneObjects[TOTAL_SCENE_OBJECTS], 
    int TotalSceneObjects,
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
        return vec3(0);
    }


    vec3 LitColor = vec3(0);

    
    

    for(int i = 0; i < TotalSceneLights; i++)
    {
        if(SceneLights[i].Intensity <= 0) continue;

        vec3 ToLightSource = vec3(0);
        
        if( SceneLights[i].Type == LIGHT_POINT)
        {
            ToLightSource = SceneLights[i].Position - HitResult.HitLocation;

        }
        else if( SceneLights[i].Type == LIGHT_DIRECTIONAL )
        {
            ToLightSource = -SceneLights[i].Direction;
        }
        else continue;

        if( SceneLights[i].bCastShadow )
        {
            
            if( 
                ShadowTrace(
                    SceneObjects,
                    TotalSceneObjects,
                    HitResult.HitLocation + 0.001,
                    normalize(ToLightSource)
                )  
                <= length(ToLightSource)
            ) continue;

           
        }

        float Attenuation = 1;

        //Light attenuation for non-directional lights
        if( SceneLights[i].Type != LIGHT_DIRECTIONAL )
        {
            float Distance    = length(ToLightSource);
            Attenuation  = Distance * Distance * SceneLights[i].Attenuation_Quadratic ;
            Attenuation += Distance * SceneLights[i].Attenuation_Linear;
            Attenuation += 1.0; //constant
            Attenuation  = 1.0 / Attenuation;

            //return vec3(Attenuation);
        }
        
        ToLightSource = normalize(ToLightSource);

        //Diffuse calculation
        float N_Dot_D             = dot(HitResult.HitNormal, ToLightSource);
        float DiffuseDistribution = max(N_Dot_D, 0.0) * SceneLights[i].Intensity;
        
        LitColor += SceneObjects[HitResult.ObjectIndex].Color 
                  * clamp( SceneObjects[HitResult.ObjectIndex].Diffuseness, 0.0, 1.0) //clamp btw 0 and 1
                  * (SceneLights[i].Color * DiffuseDistribution * Attenuation);
        
        //Specular calculation
        if( SceneObjects[HitResult.ObjectIndex].Specularity > 0.0 )
        {
            // Blin-phong specular 
            //vec3 HalfwayDir = normalize( -RayDirection + ToLightSource ); 
            //float Half_Dot_Normal = max( dot(HalfwayDir, HitResult.HitNormal), 0.0 );

            //phong
            vec3  ReflectedLightRay = reflect(-ToLightSource, HitResult.HitNormal);
            float Reflected_Dot_Eye = max( dot(-RayDirection, ReflectedLightRay), 0.0 );

            
            float SpecularDistribution = pow( Reflected_Dot_Eye, SceneObjects[HitResult.ObjectIndex].Specularity ) 
                                       * SceneLights[i].Intensity;

            LitColor += SceneObjects[HitResult.ObjectIndex].Color  
                      * (SceneLights[i].Color * SpecularDistribution * Attenuation );
        }   

        //Ambient factor 
        LitColor += SceneObjects[HitResult.ObjectIndex].Color * SceneLights[i].Ambient * Attenuation;  
    }
            
    return LitColor;
}

void Reflect(
    Object SceneObjects[TOTAL_SCENE_OBJECTS], 
    int TotalSceneObjects, 
    Light SceneLights[TOTAL_SCENE_LIGHTS],
    int TotalSceneLights,
    RenderSettings RenderSetting ,
    out vec3 InitialLitColor,
    out AHitResult OldHitResult,
    out vec3 OldRay_Direction,
    out vec3 OldRay_Position
    )
{

    int RayBounces = 0;
    while( RayBounces++ < RenderSetting.MaximumReflectionBounces )
    {
        //First make sure the previous surface hit is reflective before shooting a new ray from there
        if( SceneObjects[OldHitResult.ObjectIndex].Reflectivity > 0.0 )
        {
            vec3 NewRay_Position  = OldHitResult.HitLocation;
            vec3 NewRay_Direction = reflect(OldRay_Direction, OldHitResult.HitNormal) + OldHitResult.HitNormal;
            
            AHitResult NewHitResult = TraceScene(SceneObjects, TotalSceneObjects, NewRay_Position , NewRay_Direction);
            if(NewHitResult.bWasAHit)
            {
                
                //Basically if the newly hit object fully reflects it's environment a 100%
                //there's no point mixing it's color with the previous one since
                //this new surface itself gets it's color from somewhere else in the scene from light bounces.
                //And it may not be physically / visually correct to mix it the previous color with this object's color either
                //since "it gets it's color from somewhere else in the scene"

                //But unfortunately you might run into a visual artifact where you get some black spots on your final reflection,
                //this is because we exceeded the maximum bounce count before getting the color
                //this surface (100% reflective) was suppose to get.
                //So increasing the bounce count will solve it at the expense of more computations
                //or you can avoid 100% reflectivity so your object at least have it's own color to mix with previous color
               
                if(SceneObjects[NewHitResult.ObjectIndex].Reflectivity < 1.0) 
                {
                    vec3 SurfaceHit_LitColor = GetLitColor(
                        SceneLights,
                        TotalSceneLights,
                        SceneObjects,
                        TotalSceneLights,
                        NewHitResult,
                        NewRay_Direction,
                        RenderSetting
                    );
                        
                        
                    InitialLitColor = mix(
                        InitialLitColor, 
                        SurfaceHit_LitColor, 
                        clamp(SceneObjects[OldHitResult.ObjectIndex].Reflectivity, 0.0, 1.0)
                    );
                }
                
                OldHitResult = NewHitResult;
                OldRay_Position  = NewRay_Position;
                OldRay_Direction = NewRay_Direction;
            }
            else 
            {
                InitialLitColor = mix(
                    InitialLitColor, 
                    texture(iCubemap, NewRay_Direction).rgb ,
                    clamp(SceneObjects[OldHitResult.ObjectIndex].Reflectivity, 0.0, 1.0)
                  );

        

                break;
            }
        }
    }
   
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
//LIGHT_END
