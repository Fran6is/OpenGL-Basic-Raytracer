#pragma once
#include <iostream>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class ECameraType
{
	Perspective,
	Orthographic
};
struct FCameraDefinition
{
	ECameraType CameraType = ECameraType::Perspective;

	float FOV{60.f};
	float MaxFOV = 90.f;

	float OrthographicWidth = 100.f;

	static const char* CameraTypeAsName(ECameraType Type)
	{
		switch (Type)
		{
		case ECameraType::Perspective:  return "Perspective";
		case ECameraType::Orthographic: return "Orthographic";
		
		default: return  "Nones";
		}
	}
};

class Camera
{
    friend class ACameraController;

	FCameraDefinition CameraDefinition;

    glm::vec3 XVector  = glm::vec3(0);
	glm::vec3 YVector  = glm::vec3(0);
    glm::vec3 ZVector  = glm::vec3(0);
    glm::vec3 CameraPosition = glm::vec3(0, 0, -10);

	float Yaw   {};
	float Pitch {};

public:
	Camera();
	Camera(const FCameraDefinition& CameraDefinition);

	glm::mat3 GetBasis() const;
	float GetFOV() const { return  CameraDefinition.FOV; }
    const glm::vec3& GetCameraXVector() const{ return XVector; }
    const glm::vec3& GetCameraYVector() const{ return YVector; }
    const glm::vec3& GetCameraZVector() const{ return ZVector; }
    const glm::vec3& GetCameraPosition() const{ return CameraPosition; }
	FCameraDefinition& GetCameraSpecification() { return CameraDefinition; }

	static float NearPlane;
	static float FarPlane;
};
