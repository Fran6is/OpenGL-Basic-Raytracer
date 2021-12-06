#pragma once

#include <glm/glm.hpp>
#include <iostream>


std::ostream& operator<<(std::ostream& cout, const glm::vec3& vector);
std::ostream& operator<<(std::ostream& cout, const glm::vec4& vector);
std::ostream& operator<<(std::ostream& cout, const glm::mat3& Matrix);
std::ostream& operator<<(std::ostream& cout, const glm::mat4& Matrix);
