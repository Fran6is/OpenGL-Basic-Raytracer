#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <string>

struct SaveBase
{
private:
    bool GetFloat(const char* SlotName, const char* VariableName,   float& Out_Float) const;
    bool GetVector3(const char* SlotName, const char* VariableName, glm::vec3& Out_Vector) const;
    bool GetString(const char* SlotName, const char* VariableName,  std::string& Out_String) const;

public:
    virtual bool Save(const char* SlotName) = 0;
    virtual bool Load(const char* SlotName) = 0;
    virtual ~SaveBase(){};
};

