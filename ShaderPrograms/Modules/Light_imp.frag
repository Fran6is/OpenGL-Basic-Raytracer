WHOLE_START

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
            float D = ShadowTrace(
                    SceneObjects,
                    TotalSceneObjects,
                    HitResult.HitLocation,
                    normalize(ToLightSource)
                )  ;

            // AHitResult Hit = TraceScene(SceneObjects, TotalSceneObjects, HitResult.HitLocation , normalize(ToLightSource));
            // float D = Hit.bWasAHit ? Hit.Distance : SHADOW_TRACE_NO_HIT;

            if( D <= ( SceneLights[i].Type == LIGHT_DIRECTIONAL ? SHADOW_TRACE_NO_HIT - 1.0 : length(ToLightSource) ) )
            {
                LitColor += SceneObjects[HitResult.ObjectIndex].Color * 0.001;
                continue;
            }

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
                    RenderSetting.bUseSkyBox ? texture(iCubemap, NewRay_Direction).rgb : EnvironmentColor,
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
    
    return SHADOW_TRACE_NO_HIT;
}

WHOLE_END