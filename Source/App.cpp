#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Texture.h>
#include <Buffer.h>

#include <Path.h>
#include <RaytracingContainers.h>
#include "CameraController.h"
#include <FileEditing.h>

#define   FORCE_USE_NVIDIA 0

#if       FORCE_USE_NVIDIA
extern "C" {_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; }
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float DeltaTime);

#define NAME(X) #X


// settings
unsigned int SCR_WIDTH  = 800;
unsigned int SCR_HEIGHT = 600;

std::ostream& operator<<(std::ostream& cout, const glm::vec3& vector);
std::ostream& operator<<(std::ostream& cout, const glm::vec4& vector);
std::ostream& operator<<(std::ostream& cout, const glm::mat3& Matrix);
std::ostream& operator<<(std::ostream& cout, const glm::mat4& Matrix);



ACameraController* CameraControllerRef = nullptr;

int main()
{
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


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
    
   
    //Object buffer
    ObjectBuffer QuadBuffer;
    QuadBuffer.Prepare();

    Shader Shader_Geometry(
        GetFullPath("/ShaderPrograms/_VertexShader.vert"), 
        GetFullPath("/ShaderPrograms/0GeometryPass.frag")
    );

    
    // Shader Shader_LightCalculation(
    //     GetFullPath("/ShaderPrograms/_VertexShader.vert"),
    //     GetFullPath("/ShaderPrograms/1LightCalculationPass.frag")
    // );
    
    float DeltaTime = 0;
    float LastTime  = 0;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, DeltaTime);
        glClearColor(0, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        CameraController.Tick();

        //(1) Geometry
        Shader_Geometry.Use();
        Shader_Geometry.SetMat3("ICameraBasis",       CameraController.GetCameraBasis());
        Shader_Geometry.SetVector3("ICameraPosition", CameraController.GetCameraPosition());
        Shader_Geometry.SetFloat("ICameraFOV",        CameraController.GetCameraFOV());

        QuadBuffer.BindVAO();
        glDrawElements(GL_TRIANGLES, QuadBuffer.Get_EBO_Count(), GL_UNSIGNED_INT, 0);

        //(2) Light

        float CurrentTime = (float)glfwGetTime();
        DeltaTime = CurrentTime - LastTime;
        LastTime  = CurrentTime;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, float DeltaTime)
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH  = width;
    SCR_HEIGHT = height;
}

std::ostream& operator<<(std::ostream& cout, const glm::vec3& vector)
{
    printf("< %3.2f, %3.2f, %3.2f >", vector.x, vector.y, vector.z);
    return cout;
}

std::ostream& operator<<(std::ostream& cout, const glm::vec4& vector)
{
    printf("< %3.2f, %3.2f, %3.2f, %3.2f >", vector.x, vector.y, vector.z, vector.w);

    return cout;
}

std::ostream& operator<<(std::ostream& cout, const glm::mat3& Matrix)
{
    std::cout << "========================================MATRIX==========================================\n";
    printf("   X|   Y|    Z|  \n");
    printf("%3.2f, %3.2f, %3.2f\n", Matrix[0][0], Matrix[1][0], Matrix[2][0] );
    printf("%3.2f, %3.2f, %3.2f\n", Matrix[0][1], Matrix[1][1], Matrix[2][1] );
    printf("%3.2f, %3.2f, %3.2f\n", Matrix[0][2], Matrix[1][2], Matrix[2][2] );
    cout << "-------------------------------\n";
    cout << "X = " << Matrix[0] << "\n";
    cout << "Y = " << Matrix[1] << "\n";
    cout << "Z = " << Matrix[2] << "\n";
    std::cout << "=======================================================================================\n";

    return cout;
}

std::ostream& operator<<(std::ostream& cout, const glm::mat4& Matrix)
{
    std::cout << "========================================MATRIX==========================================\n";
    printf("   X|   Y|    Z|     W|  \n");
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][0], Matrix[1][0], Matrix[2][0], Matrix[3][0] );
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][1], Matrix[1][1], Matrix[2][1], Matrix[3][1] );
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][2], Matrix[1][2], Matrix[2][2], Matrix[3][2] );
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][3], Matrix[1][3], Matrix[2][3], Matrix[3][3] );
    cout << "-------------------------------\n";
    cout << "X = " << Matrix[0] << "\n";
    cout << "Y = " << Matrix[1] << "\n";
    cout << "Z = " << Matrix[2] << "\n";
    cout << "W = " << Matrix[3] << "\n";
    std::cout << "=======================================================================================\n";


    return cout;
}
