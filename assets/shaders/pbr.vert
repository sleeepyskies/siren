#version 460 core

// ==================================
// Attributes
// ==================================
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTextureUV;
layout (location = 5) in vec4 aColor;

// ==================================
// Uniform Buffers
// ==================================
struct PointLight {
    vec3 position;
    vec3 color;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct SpotLight {
    vec3 position;
    float innerCone;
    vec3 color;
    float outerCone;
};

layout (std140, binding = 0) uniform CameraBuffer {
    mat4 projectionView;
    vec3 cameraPosition;
    float _pad0;
};

layout (std140, binding = 1) uniform PointLights {
    PointLight pointLights[16];
    DirectionalLight directionalLights[16];
    SpotLight spotLights[16];
    int pointLightCount;
    int directionalLightCount;
    int spotLightCount;
    int _pad1;
};

// ==================================
// Required Uniforms
// ==================================
uniform mat4 uModel;

// ==================================
// Material
// ==================================
uniform sampler2D u_baseColorMap;
uniform vec4 u_baseColorFactor;

uniform sampler2D u_metallicRoughnessMap;
uniform float u_metallicFactor;
uniform float u_roughnessFactor;

uniform sampler2D u_emissionMap;
uniform vec4 u_emissionColor;

uniform sampler2D u_occlusionMap;
uniform float u_occlusionStrength;

uniform sampler2D u_normalMap;
uniform float u_normalScale;

// ==================================
// Flags
// ==================================
uniform uint u_materialFlags;

const uint HAS_BASE_COLOR_MAP          = 1u << 0;
const uint HAS_METALLIC_ROUGHNESS_MAP  = 1u << 1;
const uint HAS_EMISSION_MAP            = 1u << 3;
const uint HAS_OCCLUSION_MAP           = 1u << 4;
const uint HAS_NORMAL_MAP              = 1u << 5;


// ==================================
// Outputs
// ==================================
out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec2 v_uv;
out vec4 v_color;

void main()
{
    // matrix multiplication is right to left
    v_position = vec3(uModel * vec4(aPosition, 1.f));
    gl_Position = projectionView * vec4(v_position, 1.f);

    v_normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    v_tangent = normalize(mat3(transpose(inverse(uModel))) * aTangent);
    v_bitangent = normalize(mat3(transpose(inverse(uModel))) * aBitangent);
    v_uv = aTextureUV;
    v_color = aColor;
}
