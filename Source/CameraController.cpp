#include "CameraController.h"

#include <GlmTransformsPrint.h>

glm::mat3 ACameraController::GetCameraBasis3() const
{
    if(MountedCamera)  return glm::mat3(CameraXVector, CameraYVector, CameraZVector);
    
    return glm::mat3(1.0);
}

glm::vec3 ACameraController::GetCameraPosition() const
{
    if(MountedCamera) return CameraPosition;
    
    return glm::vec3(0);
}

float ACameraController::GetCameraFOV() const
{
    if(MountedCamera) return MountedCamera->GetFOV();
    
    return 90.f;
}

void ACameraController::MountACamera(Camera* Camera)
{
	MountedCamera = Camera;
    if (MountedCamera)
    {
        CurrentRotation.Yaw   = MountedCamera->Yaw;
        CurrentRotation.Pitch = MountedCamera->Pitch;
        CameraPosition = MountedCamera->CameraPosition;

        CalculateCameraBasis();
    }
    else
    {
        CurrentRotation.Yaw = CurrentRotation.Pitch = 0;
        CalculateCameraBasis();
    }
}

void ACameraController::Tick()
{
	if (!MountedCamera)
    {
	    std::cerr << "ACameraController::Tick - Can't control a Camera without mounting one! \n";
	    return;
    }

    //std::cout << CameraPosition << "\t Yaw = " << CurrentRotation.Yaw << "\t pitch = " << CurrentRotation.Pitch << "\n";

    if(LeftMouseButtonIsDown())
    {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        MountedCamera->XVector = CameraXVector;
        MountedCamera->YVector = CameraYVector;
        MountedCamera->ZVector = CameraZVector;
        MountedCamera->CameraPosition = CameraPosition;

        MountedCamera->Yaw   = CurrentRotation.Yaw;
        MountedCamera->Pitch = CurrentRotation.Pitch;
    }
    else    
    {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void ACameraController::Statics_MoveForwardBackward(float Scale, float DeltaTime)
{
    if(!CameraControllerRef || !CameraControllerRef->MountedCamera) return;
    
    float Speed = CameraControllerRef->TranslationalSpeed * Scale * DeltaTime;

    CameraControllerRef->CameraPosition += CameraControllerRef->CameraZVector * Speed;

    //std::cout << "Move Forward or Backward = " << Scale << "\t DeltaTime = " << DeltaTime << "\n";
    //std::cout << CameraControllerRef->CameraPosition << "\n";
}

void ACameraController::Statics_MoveLeftRight(float Scale, float DeltaTime)
{
    if(!CameraControllerRef || !CameraControllerRef->MountedCamera) return;

    float Speed = CameraControllerRef->TranslationalSpeed * Scale * DeltaTime;

    CameraControllerRef->CameraPosition += CameraControllerRef->CameraXVector * Speed;

    //std::cout << "Move Left or Right = " << Scale << "\t DeltaTime = " << DeltaTime << "\n";

}

void ACameraController::Statics_MoveUpDown(float Scale, float DeltaTime)
{
    if(!CameraControllerRef || !CameraControllerRef->MountedCamera) return;

    float Speed = CameraControllerRef->TranslationalSpeed * Scale * DeltaTime;

    CameraControllerRef->CameraPosition += CameraControllerRef->GlobalUp * Speed;

    //std::cout << "Move up or down = " << Scale << "\t DeltaTime = " << DeltaTime << "\n";
}

void ACameraController::Statics_FOV(GLFWwindow* Window, double XOffset, double YOffset) 
{
    if(!CameraControllerRef || !CameraControllerRef->MountedCamera) return;

    const float MinFOV = 0.1f;

    FCameraDefinition& ControlledCameraSpec = CameraControllerRef->GetControlledCamera()->GetCameraSpecification();
    ControlledCameraSpec.FOV -= static_cast<float>(YOffset);
    ControlledCameraSpec.FOV = glm::clamp(ControlledCameraSpec.FOV, MinFOV, ControlledCameraSpec.MaxFOV);
    
    std::cout << "FOV: " << ControlledCameraSpec.FOV << "\n";

}

void ACameraController::Statics_OnMousePositionChange(GLFWwindow* Window, double MouseX, double MouseY)
{
    if(!CameraControllerRef || !CameraControllerRef->MountedCamera) return;

    if (CameraControllerRef->MouseInfo.bFirstTime)
    {
        CameraControllerRef->MouseInfo.LastX = MouseX;
        CameraControllerRef->MouseInfo.LastY = MouseY;
        CameraControllerRef->MouseInfo.bFirstTime = false;
    }

	float DeltaX = static_cast<float>(MouseX - CameraControllerRef->MouseInfo.LastX);
	float DeltaY = static_cast<float>(MouseY - CameraControllerRef->MouseInfo.LastY);

	DeltaY *= -1.f; //y invert


	//Basically don't apply a Delta Rotation if the left mouse button is released, 
	//but do update the mouse position to the current values so we don't suddenly "jump rotate" on the next click because the delta angle was large
	if (CameraControllerRef->LeftMouseButtonIsDown()) 
	{
        //We want the rotation rate to depend on the FOV scale. Without it, the camera rotates
        //too fast when zoomed in and too slow when zoomed out
        float FOVScale = CameraControllerRef->GetControlledCamera()->GetCameraSpecification().FOV
                / CameraControllerRef->GetControlledCamera()->GetCameraSpecification().MaxFOV; 

		CameraControllerRef->CurrentRotation.Yaw   += DeltaX * (CameraControllerRef->MouseSensitivity * FOVScale);
		CameraControllerRef->CurrentRotation.Pitch += DeltaY * (CameraControllerRef->MouseSensitivity * FOVScale);
        CameraControllerRef->CurrentRotation.Pitch = glm::clamp(CameraControllerRef->CurrentRotation.Pitch, -89.f, 89.f);

	    CameraControllerRef->CalculateCameraBasis();
	}

	CameraControllerRef->MouseInfo.LastX = MouseX;
	CameraControllerRef->MouseInfo.LastY = MouseY;
}

void ACameraController::CalculateCameraBasis()
{
    if(!MountedCamera) return;

    //For this application, we choose positive Z to point towards the screen. Y is up and X is sideways.
    //If looking down from the Y axis (assuming at the world origin), then turning right is positive
    //and turning left is negative.
    //Which means when calculating all basis vectors through trigonometry (assuming only yaw); 
    //Then z component of our ZBasis vector is the adjacent side, x component of ZBasis is the opposite side.
    
    //Example(1): Yaw of 0
        //Z.z = cos(0) = 1
        //Z.x = sin(0) = 0
        //Z.y = 0
    //Example(2): Yaw of 45
        //Z.z = cos(45) = 0.71
        //Z.x = sin(0)  = 0.71
        //Z.y = 0
    //Example(3): Yaw of -45
        //Z.z = cos(45) =  0.71
        //Z.x = sin(0)  = -0.71
        //Z.y = 0
    //What about pitch calculation. Same as always, put it on the y component and multiply the other components (z and x)
    //by the sin(pitch).
    //Finally the get the other basis (X and Y), do a cross product.

    //Okay...but how will this work with mouse delta
    //With the mouse coordinate starting at the top left. We know that moving the mouse to the right(yaw)
    //yields a positive rotation which will rotate the camera right And rotating left will be negative.
    //For delta y usually will be positive (pitch) rotation when moving the mouse down and negative when
    //moving up. But we're inverting it that in "OnMousePosition" callback

    CameraZVector.z = cos(glm::radians(CurrentRotation.Yaw)) * std::cos(glm::radians(CurrentRotation.Pitch));
    CameraZVector.x = sin(glm::radians(CurrentRotation.Yaw)) * std::cos(glm::radians(CurrentRotation.Pitch));
    CameraZVector.y = sin(glm::radians(CurrentRotation.Pitch));
    CameraZVector   = glm::normalize(CameraZVector);

    CameraXVector   = glm::normalize(glm::cross(GlobalUp, CameraZVector));

    CameraYVector   = glm::normalize(glm::cross(CameraZVector, CameraXVector));
}

void ACameraController::SetMouseSensitivity(const float& Sensitivity)
{
	MouseSensitivity = glm::clamp(Sensitivity, 0.f, 1.0f);
}

void ACameraController::SetTranslationalSpeed(const float& SpeedPerSecond)
{
	TranslationalSpeed = SpeedPerSecond;
}

bool ACameraController::LeftMouseButtonIsDown() const
{
    return  glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

ACameraController* ACameraController::CameraControllerRef = nullptr;
