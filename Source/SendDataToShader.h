#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class  ACameraController;
class  Shader;
struct RenderSettings;
struct Light;
struct Object;



void SendRenderDataToShader( const ACameraController& CameraController, const Shader& ShaderProgram );
void SendRenderDataToShader( const std::vector<Light>& SceneLights, const Shader& ShaderProgram );
void SendRenderDataToShader( const RenderSettings& RenderSetting, const Shader& ShaderProgram );
void SendRenderDataToShader( const std::vector<Object>& SceneObjects, const Shader& ShaderProgram );

void SendRenderDataToShader_LightsPostProcess( const std::vector<Light>& SceneLights, const Shader& PostProcessShader );
