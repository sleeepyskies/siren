#version 460 core

in vec3 vPosition;
in vec3 vColor;
in vec3 vNormal;
in vec2 vUv;

// ==================================
// Transformation Matrices
// ==================================
uniform mat4 uModel;
uniform mat4 uProjView;

// ==================================
// Texture Maps
// ==================================
uniform sampler2D uAmbientMap;
uniform sampler2D uDiffuseMap;
uniform sampler2D uSpecularMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAlphaMap;

// ==================================
// PBR
// ==================================
uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uRoughness;
uniform float uAlpha;

// ==================================
// Flags for parameters
// ==================================
uniform bool hasAmbientMap;
uniform bool hasDiffuseMap;
uniform bool hasSpecularMap;
uniform bool hasRoughnessMap;
uniform bool hasAlphaMap;

uniform bool hasAmbientColor;
uniform bool hasDiffuseColor;
uniform bool hasSpecularColor;
uniform bool hasRoughnessValue;
uniform bool hasAlphaValue;

// ==================================
// Outputs
// ==================================
out vec4 fragColor;

void main()
{
    if (hasDiffuseMap) {
        vec4 texColor = texture(uDiffuseMap, vUv);
        fragColor = texColor * vec4(hasDiffuseColor ? uDiffuseColor : vec3(1.0), 1.0);
    } else {
        fragColor = vec4(hasDiffuseColor ? uDiffuseColor : vec3(1.0), 1.0);
    }
}