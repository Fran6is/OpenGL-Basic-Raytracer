WHOLE_START

COMMON_H

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

COMMON_IMP

WHOLE_END