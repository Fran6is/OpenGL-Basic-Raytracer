WHOLE_START

AHitResult TraceScene (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection)
{
    AHitResult FinalHitResult;
    FinalHitResult.bWasAHit = false;
    
    float Closest_Distance = -1;
    bool  bFirstTimeCheck  = true;

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

WHOLE_END