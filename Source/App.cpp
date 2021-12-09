
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "GlmTransformsPrint.h"

#include <Shader.h>
#include <Cubemap.h>
#include <ImageLoader.h>
#include <Buffer.h>

#include <Path.h>
#include <RaytracingSceneStructs.h>
#include "CameraController.h"
#include <FilesCopy.h>
#include <SendDataToShader.h>
#include <Framebuffer.h>
#include <array>

#define   FORCE_USE_NVIDIA 0

#if       FORCE_USE_NVIDIA
extern "C" {_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; }
#endif


//
void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow *window, float DeltaTime);

void AddObjectToScene(std::vector<Object>& SceneObjects, const Object& NewObject);
void AddLightToScene(std::vector<Light>& SceneLights, const Light& NewLight);

/* Globals */
unsigned int SCR_WIDTH  = 800;                         //Accessed by functions 'FramebufferResizeCallback'
unsigned int SCR_HEIGHT = 600;                         //Accessed by functions 'FramebufferResizeCallback'
std::vector< const Shader* > ShaderProgramsWindowResize;        //Accessed by functions 'FramebufferResizeCallback', 'main', and 'PrepareFramebufferWithTwoTextureAttachment()'
/**********/

int main()
{

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Geometry.frag", 
    //     "/ShaderPrograms/Source/Geometry_main.frag", 
    //     {"//WHOLE_START", "//WHOLE_END"}, 
    //     "//PASTE",
    //     false
    // );

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Geometry.frag", 
    //     "/ShaderPrograms/Source/Common.frag", 
    //     {"//COMMON_HEADER_START", "//COMMON_HEADER_END"}, 
    //     "//COMMON_HEADER",
    //     false
    // );

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Geometry.frag", 
    //     "/ShaderPrograms/Source/Common.frag", 
    //     {"//COMMON_DEF_START", "//COMMON_DEF_END"}, 
    //     "//COMMON_DEF"
    // );

    // //LIGHT

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Light.frag", 
    //     "/ShaderPrograms/Source/Light_main.frag", 
    //     {"//WHOLE_START", "//WHOLE_END"}, 
    //     "//PASTE",
    //     false
    // );

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Light.frag", 
    //     "/ShaderPrograms/Source/Common.frag", 
    //     {"//COMMON_HEADER_START", "//COMMON_HEADER_END"}, 
    //     "//COMMON_HEADER",
    //     false
    // );

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Light.frag", 
    //     "/ShaderPrograms/Source/Common.frag", 
    //     {"//LIGHT_HEADER_START", "//LIGHT_HEADER_END"}, 
    //     "//LIGHT_HEADER",
    //     false
    // );

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Light.frag", 
    //     "/ShaderPrograms/Source/Common.frag", 
    //     {"//COMMON_DEF_START", "//COMMON_DEF_END"}, 
    //     "//COMMON_DEF",
    //     false
    // );

    // CopyPasteFileSection(
    //     "/ShaderPrograms/Compile/Light.frag", 
    //     "/ShaderPrograms/Source/Common.frag", 
    //     {"//LIGHT_DEF_START", "//LIGHT_DEF_END"}, 
    //     "//LIGHT_DEF"
    // );

    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Raytracing", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    //Camera setup / definition
    Camera MainCamera;

    ACameraController CameraController;
    CameraController.MountACamera(&MainCamera);
    CameraController.Window = window; 
    CameraController.SetTranslationalSpeed(10);
    CameraController.SetMouseSensitivity(0.15f);
    glfwSetCursorPosCallback(window, &ACameraController::Statics_OnMousePositionChange);
    glfwSetScrollCallback(window, &ACameraController::Statics_FOV);
    
    //Vertex buffer creation
    QuadBuffer aQuadBuffer;
    aQuadBuffer.Prepare();

    //Shader program(s)
    Shader Shader_Geometry(
        GetFullPath("/ShaderPrograms/0VertexShader.vert"), 
        GetFullPath("/ShaderPrograms/1GeometryPass.frag")
    );

    Shader Shader_LightCalculation(
        GetFullPath("/ShaderPrograms/0VertexShader.vert"),
        GetFullPath("/ShaderPrograms/2LightCalculationPass.frag")
    );

    Shader Shader_PostProcess(
        GetFullPath("/ShaderPrograms/0VertexShader.vert"),
        GetFullPath("/ShaderPrograms/Compile/PostProcessPass.frag")
    );

    ShaderProgramsWindowResize.emplace_back(&Shader_Geometry);
    ShaderProgramsWindowResize.emplace_back(&Shader_LightCalculation);
    ShaderProgramsWindowResize.emplace_back(&Shader_PostProcess);

    FramebufferResizeCallback(window, SCR_WIDTH, SCR_HEIGHT);


    //Framebuffer setup
    FTexImage TexImage2dParams;
    TexImage2dParams.TargetTextureType = GL_TEXTURE_2D;
    TexImage2dParams.InternalFormat    = GL_RGBA16F;
    TexImage2dParams.Width    = SCR_WIDTH;
    TexImage2dParams.Height   = SCR_HEIGHT;
    TexImage2dParams.Format   = GL_RGBA;
    TexImage2dParams.DataType = GL_FLOAT;
    TexImage2dParams.Data     = NULL;

    FTextureParameters TextureParams;
    TextureParams.UpSamplingFunction   = ETextureResamplingFunction::NearestNeighbor;
    TextureParams.DownSamplingFunction = ETextureResamplingFunction::NearestNeighbor;

    std::vector<const char*> AttachmentNames = {"GPosition", "GNormal"};
    Framebuffer GFramebuffer
    ( 
        AttachmentNames,
        TexImage2dParams, 
        TextureParams 
    );
    GFramebuffer.AddFramebufferForWindowResizeCallback(&GFramebuffer);
    GFramebuffer.SelectAttachmentsToDrawTo(AttachmentNames);

    GLuint GPositionTex = GFramebuffer.GetTextureAttachmentID("GPosition");
    GLuint GNormalTex   = GFramebuffer.GetTextureAttachmentID("GNormal");

    //Scene objects
    std::vector<Object> SceneObjects; SceneObjects.reserve(5);
    Object SceneDefaultObject = {OBJECT_SPHERE};
    //     //object1
    // SceneDefaultObject.Position = glm::vec3(0, 0, 5);
    // SceneDefaultObject.Color    = glm::vec3(0,0, 1);
    // SceneDefaultObject.Scale    = 6;
    // SceneDefaultObject.Specularity = 200;
    // SceneDefaultObject.Diffuseness = 0.5f;
    // SceneDefaultObject.Reflectivity = .75f;
    // AddObjectToScene(SceneObjects, SceneDefaultObject);
    //     //object2
    SceneDefaultObject.Type  = OBJECT_SPHERE;
    SceneDefaultObject.Position = glm::vec3(0, 5, 0);
    SceneDefaultObject.Color    = glm::vec3(0, 1, 0);
    SceneDefaultObject.Scale    = 3;
    SceneDefaultObject.Specularity = 100;
    SceneDefaultObject.Diffuseness = 0.125f;
    SceneDefaultObject.Reflectivity = .75f;
    AddObjectToScene(SceneObjects, SceneDefaultObject);

        //object3
    SceneDefaultObject.Type     = OBJECT_PLANE;
    SceneDefaultObject.Position = glm::vec3(0);
    SceneDefaultObject.Color    = glm::vec3(0, 1, 0);
    SceneDefaultObject.Rotation = vec2(0, 0);
    SceneDefaultObject.Scale    = 8;
    SceneDefaultObject.Specularity  = 100;
    SceneDefaultObject.Diffuseness  = 0.8f;
    SceneDefaultObject.Reflectivity = .0f;
    AddObjectToScene(SceneObjects, SceneDefaultObject);

    //Scene lights
    std::vector<Light> SceneLights; SceneLights.reserve(3);
    Light SceneDefaultLight;
    
    // SceneDefaultLight.Type = LIGHT_DIRECTIONAL;
    // SceneDefaultLight.Color     = vec3(1);
    // SceneDefaultLight.Ambient   = SceneDefaultLight.Color * 0.25f;
    // SceneDefaultLight.Direction  = vec3(50, -2, 0);
    // SceneDefaultLight.Intensity = 1.f;
    // SceneDefaultLight.Attenuation_Linear    = 0.0125f;
    // SceneDefaultLight.Attenuation_Quadratic = 0.f;
    // AddLightToScene(SceneLights, SceneDefaultLight);
    
    SceneDefaultLight.Type = LIGHT_POINT;
    SceneDefaultLight.Color     = vec3(1, 0, 0);
    SceneDefaultLight.Ambient   = SceneDefaultLight.Color * 0.25f; 
    SceneDefaultLight.Position  = vec3(0, 10 , 0);
    SceneDefaultLight.Intensity = 30.f;
    SceneDefaultLight.Attenuation_Linear    = 0.0125f;
    SceneDefaultLight.Attenuation_Quadratic = 0.00625f;
    AddLightToScene(SceneLights, SceneDefaultLight);


    //Render settings
    RenderSettings RenderSetting;
    RenderSetting.ShadingType = SHADING_DIFFUSE_REFLECT;  
    RenderSetting.MaximumReflectionBounces = 5;

    //Send in data (Objects, Lights, Rendersettings)
    Shader_Geometry.Use();
    Shader_Geometry.SetInt("iGPosition", 2);
    Shader_Geometry.SetInt("iGNormal",   3);
    SendRenderDataToShader(SceneObjects, Shader_Geometry);
    SendRenderDataToShader(CameraController, Shader_Geometry);

    Shader_LightCalculation.Use();
    Shader_LightCalculation.SetInt("iGPosition", 2);
    Shader_LightCalculation.SetInt("iGNormal",   3);
    SendRenderDataToShader(SceneObjects,     Shader_LightCalculation);
    SendRenderDataToShader(SceneLights,      Shader_LightCalculation);
    SendRenderDataToShader(RenderSetting,    Shader_LightCalculation);
    SendRenderDataToShader(CameraController, Shader_Geometry);

    Shader_PostProcess.Use();
    Shader_PostProcess.SetInt("iGPosition", 2);
    SendRenderDataToShader(CameraController, Shader_PostProcess);
    //SendRenderDataToShader_LightsPostProcess(SceneLights, Shader_PostProcess);

    //Cubemap
    std::array<std::string, 6> CubemapFaces
    {
        GetFullPath("/Textures/Cubemaps/StLazarusChurch/posx.jpg"),
        GetFullPath("/Textures/Cubemaps/StLazarusChurch/negx.jpg"),
        GetFullPath("/Textures/Cubemaps/StLazarusChurch/posy.jpg"),
        GetFullPath("/Textures/Cubemaps/StLazarusChurch/negy.jpg"),
        GetFullPath("/Textures/Cubemaps/StLazarusChurch/posz.jpg"),
        GetFullPath("/Textures/Cubemaps/StLazarusChurch/negz.jpg")
    };

    Cubemap Skybox(CubemapFaces, false);
    Shader_LightCalculation.Use();
    Shader_LightCalculation.SetInt("iCubemap", 0);

    //Delta time
    float DeltaTime = 0;
    float LastTime  = 0;

    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window, DeltaTime);
        CameraController.Tick();

            //(1) Geometry pass
            //----------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, GFramebuffer.GetID()); 
        glClearColor(0.f, 0.0f, 0.0f, 1.f); 
        glClear(GL_COLOR_BUFFER_BIT);

        Shader_Geometry.Use();
        SendRenderDataToShader(CameraController, Shader_Geometry);
        
        aQuadBuffer.BindVAO();
        glDrawElements(GL_TRIANGLES, aQuadBuffer.Get_EBO_Count(), GL_UNSIGNED_INT, 0);

            //(2) Light pass
            //----------------------------
        Texture::StaticBindTexture(GL_TEXTURE_2D, GPositionTex,  2);
        Texture::StaticBindTexture(GL_TEXTURE_2D, GNormalTex,    3);
        Skybox.Bind(0);
        Shader_LightCalculation.Use();
        SendRenderDataToShader(CameraController, Shader_LightCalculation);

        aQuadBuffer.BindVAO();
        glDrawElements(GL_TRIANGLES, aQuadBuffer.Get_EBO_Count(), GL_UNSIGNED_INT, 0);

            //(3) Post-processing
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
        glClearColor(0, 0.0f, 0.0f, 1.f); 
        glClear(GL_COLOR_BUFFER_BIT );

        Texture::StaticBindTexture(GL_TEXTURE_2D, GPositionTex,  2);
        Shader_PostProcess.Use();
        SendRenderDataToShader(CameraController, Shader_PostProcess);

        aQuadBuffer.BindVAO();
        glDrawElements(GL_TRIANGLES, aQuadBuffer.Get_EBO_Count(), GL_UNSIGNED_INT, 0);


        //
        float CurrentTime = (float)glfwGetTime();
        DeltaTime = CurrentTime - LastTime;
        LastTime  = CurrentTime;

        //std::cout << 1 / DeltaTime << "\n" ;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void AddObjectToScene(std::vector<Object>& SceneObjects, const Object& NewObject)
{
    SceneObjects.emplace_back(NewObject);
    SceneObjects.at(SceneObjects.size() - 1).Color =  glm::max(NewObject.Color, vec3(0.0095f));
    SceneObjects.at(SceneObjects.size() - 1).ID    =  SceneObjects.size() - 1;
}

void AddLightToScene(std::vector<Light>& SceneLights, const Light& NewLight)
{
    SceneLights.emplace_back(NewLight);
    SceneLights.at(SceneLights.size() - 1).Color   = glm::max(NewLight.Color,   vec3(0.0095f));
    SceneLights.at(SceneLights.size() - 1).Ambient = glm::max(NewLight.Ambient, vec3(0.0095f));
}

void ProcessInput(GLFWwindow *window, float DeltaTime)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Camera Controls

    //forward 'W'
    ACameraController::Statics_MoveForwardBackward(
        glfwGetKey(window, GLFW_KEY_W) * 1.f,
        DeltaTime
    );

    //back 'S'
    ACameraController::Statics_MoveForwardBackward(
        glfwGetKey(window, GLFW_KEY_S) * -1.0f,
        DeltaTime
    );

    //left 'A'
    ACameraController::Statics_MoveLeftRight(
        glfwGetKey(window, GLFW_KEY_A) * -1.0f,
        DeltaTime
    );

    //right 'D'
    ACameraController::Statics_MoveLeftRight(
        glfwGetKey(window, GLFW_KEY_D) * 1.0f,
        DeltaTime
    );

    //up 'E'
    ACameraController::Statics_MoveUpDown(
        glfwGetKey(window, GLFW_KEY_E) * 1.0f,
        DeltaTime
    );

    //down 'Q'
    ACameraController::Statics_MoveUpDown(
        glfwGetKey(window, GLFW_KEY_Q) * -1.0f,
        DeltaTime
    );
}

void FramebufferResizeCallback(GLFWwindow* window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);

    std::cout << "\n'FramebufferResizeCallback()'::Framebuffer(s) resized from \t[" << SCR_WIDTH << " x " << SCR_HEIGHT << "] to [" << Width << " x " << Height << "]\n";
    SCR_WIDTH  = Width;
    SCR_HEIGHT = Height;

   

    Framebuffer::OnWindowResize(Width, Height);


    if(ShaderProgramsWindowResize.size() > 0)
    {
        for(size_t I = 0; I < ShaderProgramsWindowResize.size(); I++)
        {
            if(ShaderProgramsWindowResize[I])
            {

                //std::cout << "'FramebufferResizeCallback()'::New dimension passed to shader " << I+1 << "\n";

                ShaderProgramsWindowResize[I]->Use();
                ShaderProgramsWindowResize[I]->SetVector2("IResolution", glm::vec2(Width, Height));
            }
        }
        std::cout << "\n";
    }
}