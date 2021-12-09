#include "Camera.h"



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
