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

        sprintf(buffer, "ISceneLights[%i].Ambient", i);
        ShaderProgram.SetVector3(buffer, SceneLight.Ambient );

        sprintf(buffer, "ISceneLights[%i].Direction", i);
        ShaderProgram.SetVector3(buffer, SceneLight.Direction );

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

    ShaderProgram.SetInt("IRenderSetting.MaximumReflectionBounces", RenderSetting.MaximumReflectionBounces );

    ShaderProgram.SetBool("IRenderSetting.bUseSkyBox", RenderSetting.bUseSkyBox);
    
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

        if(SceneObject.Type == OBJECT_PLANE)
        {
            const float Pitch = glm::radians( glm::clamp(SceneObject.Rotation.x, -89.f, +89.f) );
            const float Yaw   = glm::radians( SceneObject.Rotation.y );

            vec3 Z, Y, X;
            Z.z = cos( Yaw ) * cos( Pitch );
            Z.x = sin( Yaw ) * cos( Pitch );
            Z.y = sin( Pitch );

            Y = normalize( glm::cross( Z, glm::cross( glm::vec3(0, 1, 0), Z  ) ) );

            X = normalize( cross( Y, Z ) );

            sprintf(buffer, "ISceneObjects[%i].Basis", i);
            ShaderProgram.SetMat3(
                buffer, 
                mat3(X, Y, Z)
            );
        }
        
        i++;
    }
    
    ShaderProgram.SetInt("ITotalSceneObjects", SceneObjects.size());

}