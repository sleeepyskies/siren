#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTextureUV;

out vec3 vPosition;
out vec3 vColor;
out vec3 vNormal;
out vec2 vUv;

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
// Outputs
// ==================================


void main()
{
    // matrix multiplication is right to left
    vPosition = vec3(uModel * vec4(aPosition, 1.f));
    gl_Position = uProjView * vec4(vPosition, 1.f);
    vColor = aColor;
    vNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    vUv = aTextureUV;
}
