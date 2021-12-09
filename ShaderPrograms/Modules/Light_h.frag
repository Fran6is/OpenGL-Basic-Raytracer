WHOLE_START
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
    bool bUseSkyBox;
};

uniform   int    ITotalSceneLights  = 0;
uniform   Light  ISceneLights[TOTAL_SCENE_LIGHTS];
uniform   RenderSettings IRenderSetting = RenderSettings(SHADING_DIFFUSE, 0, false);

uniform samplerCube iCubemap;
vec3    EnvironmentColor = vec3(0.01);  //If no skybox

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
#define SHADOW_TRACE_NO_HIT 10000000
float ShadowTrace (Object SceneObjects[TOTAL_SCENE_OBJECTS], int TotalObjects, vec3 RayPosition, vec3 RayDirection);
WHOLE_END