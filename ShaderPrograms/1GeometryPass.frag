#version 330

//COMMON_START(Geometry and Light)
layout (location = 0) out vec4 GPosition;
layout (location = 1) out vec4 GNormal;

uniform vec2  IResolution     = vec2(800.0, 600.0);
uniform mat3  ICameraBasis    = mat3(1.0);
uniform vec3  ICameraPosition = vec3(0.0);
uniform float ICameraFOV      = 90;

struct AHitResult { bool bWasAHit; vec3 HitLocation; vec3 HitNormal; int ObjectIndex; float Distance;  };

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


void main()
{
    //Ray Construction from fragment co-ordinate
    vec2 UV = (gl_FragCoord.xy/IResolution) * 2.0 - vec2(1.0);

    vec3 RayOrigin    = ICameraPosition;
	vec3 RayDirection = vec3(UV, 1.0);
    RayDirection.x *= tan(radians(ICameraFOV/2.0)) * IResolution.x / IResolution.y;
    RayDirection.y *= tan(radians(ICameraFOV/2.0));
    RayDirection    = normalize( ICameraBasis * RayDirection );
 
    //Trace
    AHitResult HitResult = TraceScene(ISceneObjects, ITotalSceneObjects, RayOrigin, RayDirection);

    const float E = 0.25;
    if(HitResult.bWasAHit)
    {
        vec3 color = ISceneObjects[HitResult.ObjectIndex].Color;
        GPosition.rgb = HitResult.HitLocation;
        GPosition.a   = HitResult.ObjectIndex + E; 
        //will be casted back to an int in light stage of rendering to get object index.
        //okay...but why add the extra value? 
        //To avoid a case (for example) where 1 becomes 0.999; which when casted to an int in
        //the light stage, becomes 0 which is not the intended index

        GNormal.rgb = HitResult.HitNormal;
        GNormal.a   = HitResult.ObjectIndex + E;
    }
    else
    {
        GPosition.rgb = vec3(0);
        GPosition.a   = NO_HIT_ID + E;

        GNormal.rgb = vec3(0);
        GNormal.a   = NO_HIT_ID + E;
    }
}

//COMMON_START(Geometry and Light)
AHitResult TraceScene (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection)
{
    AHitResult FinalHitResult;
    FinalHitResult.bWasAHit = false;
    
    float Closest_Distance = 100000.0;
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
