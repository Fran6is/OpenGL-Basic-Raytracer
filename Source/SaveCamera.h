#pragma once

#include <SaveBase.h>

struct SaveCamera: public SaveBase
{
    int Type;
    glm::vec3 Position;
    glm::vec3 Forward;
    glm::vec3 Right;
    glm::vec3 Up;
    float     FOV;
    int OrthographicWidth;

    bool Save(const char* SlotName) override;
    bool Load(const char* SlotName) override;
};