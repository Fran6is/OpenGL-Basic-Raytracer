WHOLE_START

COMMON_H
LIGHT_H

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

        LitColor = IRenderSetting.bUseSkyBox ? texture(iCubemap, RayDirection).rgb : EnvironmentColor;

    }

    GPosition.rgb =  LitColor;
    GPosition.a   = PixelID;
}
COMMON_IMP
LIGHT_IMP
WHOLE_END