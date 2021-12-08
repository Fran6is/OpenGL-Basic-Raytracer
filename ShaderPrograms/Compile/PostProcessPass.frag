#version 330 core

struct Light
{
    vec3  Position;
    vec3  Color;
    float Intensity;
};

uniform vec2  IResolution     = vec2(800.0, 600.0);
uniform mat3  ICameraBasis    = mat3(1.0);
uniform vec3  ICameraPosition = vec3(0.0);
uniform float ICameraFOV      = 90;

const int     TOTAL_SCENE_LIGHTS  = 5;
uniform int   ITotalSceneLights   = 0;
uniform Light ISceneLights[TOTAL_SCENE_LIGHTS];

uniform sampler2D iGPosition;

in vec2 TexCoord;

float CreateCircle( vec2 Position, float Radius, out vec2 Grid );
float GetLightsAsCircles();

void main(void)
{
    float AR  = IResolution.x/IResolution.y;
    vec2 Grid = (gl_FragCoord.xy / IResolution * 2.0) - vec2(1);
    Grid.x   *= AR;

    vec3 FinalColor = texture(iGPosition, TexCoord).rgb;

    mat3 View = inverse(ICameraBasis);
    vec2 Proj = vec2( 1.0 / ( tan( radians(ICameraFOV*0.5) ) * AR), 1.0 / ( tan( radians(ICameraFOV*0.5) ) ) );

    vec3 LightsAsCircles = vec3(0);

    for(int i = 0; i < ITotalSceneLights; i++)
    {
        vec3 InCameraSpace = View * ISceneLights[i].Position;
        InCameraSpace     += View * -ICameraPosition;

        InCameraSpace.xy = InCameraSpace.xy * Proj / InCameraSpace.z;
        
        //clippping to avoid the bug below and to improve performance
        //if light xy position is beyond -1 and +1. Basically no need to draw a circle not in view
        //in x or  y. Of which we can 'skip' drawing if light is beyond -+1.
        //but then again the light might just 'vanish' when it gets to the edge -+1
        if( InCameraSpace.z <= 0 /*|| abs(InCameraSpace.x) > 1.0 || abs(InCameraSpace.y) > 1.0*/ || ISceneLights[i].Intensity <= 0) continue;

        InCameraSpace.z = 1.0 / (InCameraSpace.z + 1.0); //will be our radius
        //BUG:
        //if z is 0 or -, the expression can evaluate to some radius enough to 
        //calculate a circle even when the light is behind the camera
        //Ex. z = -0.5  which will be 1 / (-0.5 + 1)  = 0.5
        //Ex. z = -0.75 which will be 1 / (-0.75 + 1) = 0.25
        //Ex. z = -1    which will be 1 / (-1 + 1)    = 0 . not a big deal, we just simply return from the function
        //Solution: check if z is negative 

         //light is behind the camera

        LightsAsCircles += ISceneLights[i].Color 
                         * ISceneLights[i].Intensity 
                         * CreateCircle(InCameraSpace.xy, InCameraSpace.z, Grid);
    }

    FinalColor += LightsAsCircles; 
    //Exposure tone mapping
    float Exposure = 1.0;
    FinalColor = vec3(1.0) - exp(-FinalColor * Exposure);

    //Gamma correction
    float Gamma = 2.2;
    FinalColor = pow(FinalColor, vec3(1.0/Gamma));

    gl_FragColor.rgb = FinalColor; 
}

float CreateCircle( vec2 Position, float Radius, out vec2 Grid )
{
    if(Radius <= 0.0) return 0.0;

    float Circle = distance( Position, Grid ) / Radius;
    Circle = min(Circle, 1.0);
    Circle = 1.0 - Circle;
    
    return Circle;
}