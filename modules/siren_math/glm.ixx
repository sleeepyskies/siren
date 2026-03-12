module;

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

export module siren.math;

export namespace glm
{
// Vectors
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::dvec2;
using glm::dvec3;
using glm::dvec4;

// Matrices
using glm::mat2;
using glm::mat3;
using glm::mat4;

// Quaternions
using glm::quat;

// Transformations
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::lookAt;
using glm::perspective;
using glm::ortho;

// Common Math
using glm::normalize;
using glm::cross;
using glm::dot;
using glm::distance;
using glm::length;
using glm::clamp;
using glm::mix;
using glm::lerp;

// Utilities
using glm::value_ptr;
using glm::radians;
using glm::degrees;
using glm::inverse;
using glm::transpose;

// Constants
using glm::pi;
using glm::epsilon;
} // namespace glm
