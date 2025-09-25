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
// Cameras and Lighting
// ==================================
uniform vec3 uCameraPos;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
// a buffer of lighting positions, colors and types instead?

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
    // TODO: double check math here! >:D
    /*    vec3 ambient   = hasAmbientMap   ? texture(uAmbientMap, vUv).rgb   : (hasAmbientColor   ? uAmbientColor   : vec3(1.0));
        vec3 diffuse   = hasDiffuseMap   ? texture(uDiffuseMap, vUv).rgb   : (hasDiffuseColor   ? uDiffuseColor   : vec3(1.0));
        vec3 specular  = hasSpecularMap  ? texture(uSpecularMap, vUv).rgb  : (hasSpecularColor  ? uSpecularColor  : vec3(1.0));
        float rough    = hasRoughnessMap ? texture(uRoughnessMap, vUv).r   : (hasRoughnessValue ? uRoughness      : 1.0);
        float alpha    = hasAlphaMap     ? texture(uAlphaMap, vUv).r       : (hasAlphaValue     ? uAlpha          : 1.0);

        vec3 N = normalize(vNormal);
        vec3 L = normalize(uLightPos - vPosition);
        vec3 V = normalize(uCameraPos - vPosition);
        vec3 R = reflect(-L, N);

        float NdotL = max(dot(N, L), 0.0);
        vec3 diff = diffuse * NdotL;

        float shininess = max(1.0, 128.0 * (1.0 - rough));
        float spec = pow(max(dot(R, V), 0.0), shininess);
        vec3 specCol = specular * spec;

        vec3 amb = 0.1 * ambient;

        fragColor = vec4(amb + diff + specCol, alpha);*/
    fragColor = hasDiffuseMap ? texture(uDiffuseMap, vUv) : (hasDiffuseColor ? vec4(uDiffuseColor, 1) : vec4(1));
}