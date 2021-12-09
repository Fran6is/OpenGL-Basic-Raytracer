#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>

struct FMouseMovement
{
	double LastX = 0.0;
	double LastY = 0.0;
	bool bFirstTime = true;
};

struct EulerAngles
{
	float Yaw   = 0.0f;
	float Pitch = 0.0f;
};

class ACameraController
{
	
private:

	FMouseMovement MouseInfo;
	EulerAngles    CurrentRotation;

	Camera* MountedCamera = nullptr;

	glm::vec3 CameraXVector = glm::vec3(0);
	glm::vec3 CameraYVector = glm::vec3(0);
	glm::vec3 CameraZVector = glm::vec3(0);

	glm::vec3 CameraPosition = glm::vec3(0);
	glm::vec3 GlobalUp = glm::vec3(0, 1, 0);

	float MouseSensitivity   = 0.1f; //To multiply our delta mouse movement
	float TranslationalSpeed = 5.f;
	
    static ACameraController* CameraControllerRef;

public:
    GLFWwindow* Window = nullptr;

	static void Statics_OnMousePositionChange(GLFWwindow* Window, double XPosition, double YPosition);
	static void Statics_MoveForwardBackward(float Scale, float DeltaTime);
	static void Statics_MoveLeftRight(float Scale, float DeltaTime);
	static void Statics_MoveUpDown(float Scale, float DeltaTime);
	static void Statics_FOV(GLFWwindow* Window, double XOffset, double YOffset);

private:
    inline bool LeftMouseButtonIsDown() const;
	void CalculateCameraBasis();

public:

    ACameraController()
    {
       CameraControllerRef = this; 
    }
	
    void MountACamera(Camera* Camera);

	inline bool HasACamera() const{ return MountedCamera;}

	//Will multiply the delta x and y mouse displacement. Default = 0.1 ( 10% ) of delta mouse movement
	void SetMouseSensitivity(const float& Sensitivity); 

	//Translational speed (per second). Default = 5units/s
	void SetTranslationalSpeed(const float& Persecond); 

	void Tick();
	
	Camera* GetControlledCamera() { return MountedCamera; }

	//if there's a camera, return it's orientation basis else standard basis
	glm::mat3 GetCameraBasis3() const;

	glm::vec3 GetCameraPosition() const;
	float GetCameraFOV() const;
};


