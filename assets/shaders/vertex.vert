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
// Required Uniforms
// ==================================
uniform mat4 uModel;
uniform mat4 uProjView;
uniform vec3 uCameraPos;

// ==================================
// Material
// ==================================
uniform sampler2D uBaseColorMap;
uniform vec4 uBaseColorFactor;

uniform sampler2D uMetallicMap;
uniform float uMetallicFactor;

uniform sampler2D uRoughnessMap;
uniform float uRoughnessFactor;

uniform sampler2D uEmissionMap;
uniform vec4 uEmissionColor;

uniform sampler2D uOcclusionMap;
uniform float uOcclusionStrength;

uniform sampler2D uNormalMap;
uniform float uNormalScale;


// ==================================
// Flags
// ==================================
uniform uint uMaterialFlags;

const uint HAS_BASE_COLOR_MAP         = 1u << 0;
const uint HAS_METALLIC_ROUGHNESS_MAP = 1u << 1;
const uint HAS_EMISSION_MAP           = 1u << 2;
const uint HAS_OCCLUSION_MAP          = 1u << 3;
const uint HAS_NORMAL_MAP             = 1u << 4;


// ==================================
// Outputs
// ==================================
out vec3 vPosition;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vUv;
out vec4 vColor;

void main()
{
    // matrix multiplication is right to left
    vPosition = vec3(uModel * vec4(aPosition, 1.f));
    gl_Position = uProjView * vec4(vPosition, 1.f);

    vNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    vTangent = aTangent;
    vBitangent = aBitangent;
    vUv = aTextureUV;
    vColor = aColor;
}
