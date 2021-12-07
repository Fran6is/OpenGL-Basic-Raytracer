#include "SendDataToShader.h"
#include <CameraController.h>
#include <Shader.h>
#include <RaytracingSceneStructs.h>

#pragma warning(disable : 4996) //suppress 'sprintf' warning or use 'sprintf_s'


void SendRenderDataToShader_LightsPostProcess( const std::vector<Light>& SceneLights, const Shader& PostProcessShader )
{
    int i = 0;
    for (const auto &SceneLight: SceneLights)
    {
        if(SceneLight.Type == LIGHT_DIRECTIONAL) continue;

        char buffer[64]{};

        sprintf(buffer, "ISceneLights[%i].Position", i);
        PostProcessShader.SetVector3(buffer, SceneLight.Position );

        sprintf(buffer, "ISceneLights[%i].Color", i);
        PostProcessShader.SetVector3(buffer, SceneLight.Color );

        sprintf(buffer, "ISceneLights[%i].Intensity", i);
        PostProcessShader.SetFloat(buffer, SceneLight.Intensity );
        i++;
    }
    PostProcessShader.SetInt("ITotalSceneLights", SceneLights.size());
    
}

void SendRenderDataToShader( const ACameraController& CameraController, const Shader& ShaderProgram )
{
    ShaderProgram.Use();
    ShaderProgram.SetMat3("ICameraBasis",       CameraController.GetCameraBasis3());
    ShaderProgram.SetVector3("ICameraPosition", CameraController.GetCameraPosition());
    ShaderProgram.SetFloat("ICameraFOV",        CameraController.GetCameraFOV());
}

void SendRenderDataToShader( const std::vector<Light>& SceneLights, const Shader& ShaderProgram )
{
    int i = 0;
    ShaderProgram.Use();
    for (const auto SceneLight : SceneLights)
    {
        char buffer[64]{};

        sprintf(buffer, "ISceneLights[%i].Type", i);
        ShaderProgram.SetInt(buffer, SceneLight.Type );

        sprintf(buffer, "ISceneLights[%i].Position", i);
        ShaderProgram.SetVector3(buffer, SceneLight.Position );

        sprintf(buffer, "ISceneLights[%i].Color", i);
        ShaderProgram.SetVector3(buffer, SceneLight.Color );

        sprintf(buffer, "ISceneLights[%i].Direction", i);
        ShaderProgram.SetVector3(buffer, SceneLight.Direction );

        sprintf(buffer, "ISceneLights[%i].Radius", i);
        ShaderProgram.SetFloat(buffer, SceneLight.Radius );

        sprintf(buffer, "ISceneLights[%i].Intensity", i);
        ShaderProgram.SetFloat(buffer, SceneLight.Intensity );

        sprintf(buffer, "ISceneLights[%i].Attenuation_Linear", i);
        ShaderProgram.SetFloat(buffer, SceneLight.Attenuation_Linear );

        sprintf(buffer, "ISceneLights[%i].Attenuation_Quadratic", i);
        ShaderProgram.SetFloat(buffer, SceneLight.Attenuation_Quadratic );

        sprintf(buffer, "ISceneLights[%i].bCastShadow", i);
        ShaderProgram.SetBool(buffer, SceneLight.bCastShadow);

        i++;
    }
    
    ShaderProgram.SetInt("ITotalSceneLights", SceneLights.size());
    
}

void SendRenderDataToShader(const RenderSettings& RenderSetting, const Shader& ShaderProgram )
{
    ShaderProgram.Use();

    ShaderProgram.SetInt("IRenderSetting.ShadingType", RenderSetting.ShadingType );

    // ShaderProgram.SetBool("IRenderSetting.bAllowReflection", RenderSetting.bAllowReflection );

    // ShaderProgram.SetBool("IRenderSetting.bAllowRefraction", RenderSetting.bAllowRefraction );

    ShaderProgram.SetInt("IRenderSetting.MaximumReflectionBounces", RenderSetting.MaximumReflectionBounces );
    
}
void SendRenderDataToShader( const std::vector<Object>& SceneObjects, const Shader& ShaderProgram )
{
    int i = 0;
    ShaderProgram.Use();
    for (const auto SceneObject : SceneObjects)
    {
        char buffer[64]{};

        sprintf(buffer, "ISceneObjects[%i].Type", i);
        ShaderProgram.SetInt(buffer, SceneObject.Type );

        sprintf(buffer, "ISceneObjects[%i].ID", i);
        ShaderProgram.SetInt(buffer, SceneObject.ID );

        sprintf(buffer, "ISceneObjects[%i].Position", i);
        ShaderProgram.SetVector3(buffer, SceneObject.Position );

        sprintf(buffer, "ISceneObjects[%i].Scale", i);
        ShaderProgram.SetFloat(buffer, SceneObject.Scale);

        sprintf(buffer, "ISceneObjects[%i].Color", i);
        ShaderProgram.SetVector3(buffer, SceneObject.Color);

        sprintf(buffer, "ISceneObjects[%i].Diffuseness", i);
        ShaderProgram.SetFloat(buffer, SceneObject.Diffuseness);

        sprintf(buffer, "ISceneObjects[%i].Specularity", i);
        ShaderProgram.SetFloat(buffer, SceneObject.Specularity);

        sprintf(buffer, "ISceneObjects[%i].Reflectivity", i);
        ShaderProgram.SetFloat(buffer, SceneObject.Reflectivity);

        sprintf(buffer, "ISceneObjects[%i].IOR", i);
        ShaderProgram.SetFloat(buffer, SceneObject.IOR);

        i++;
    }
    
    ShaderProgram.SetInt("ITotalSceneObjects", SceneObjects.size());

}