#include "Camera.h"



float Camera::NearPlane = 0.001f;
float Camera::FarPlane  = 10000.f;

Camera::Camera() : Camera(FCameraDefinition{ ECameraType::Perspective })
{
}

Camera::Camera(const FCameraDefinition& CameraSpecs) : CameraDefinition{CameraSpecs}
{

}

glm::mat3 Camera::GetBasis() const
{
    return glm::mat3(XVector, YVector, ZVector);
}
