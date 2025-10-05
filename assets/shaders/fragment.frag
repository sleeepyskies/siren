#version 460 core

// ==================================
// Interpolated Inputs
// ==================================
in vec3 vPosition;
in vec4 vColor;
in vec3 vNormal;
in vec2 vUv;
in vec4 vTangent;

// ==================================
// Light Uniforms
// ==================================
struct PointLight {
    vec3 color;// implicit padding here, vec3 is aligned as a vec4
    vec3 position;
};

layout (std140, binding = 0) uniform PointLights {
    PointLight pointLights[16];
    int lightCount;
    int _pad[3];
};

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

// R channel := Ambient Oclussion (optional)
// G channel := Roughness
// B channel := Metallic
uniform sampler2D uMetallicRoughnessMap;
uniform float uMetallicFactor;
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
out vec4 fragColor;

const float PI = 3.14159265359;
/*
const float CONSTANT = 1;
const float LINEAR = 0.7;
const float QUADRATIC = 0.8;
const float INTENSITY = 10;
*/

vec3 getNormal() {
    if ((HAS_NORMAL_MAP & uMaterialFlags) == 0u) { return normalize(vNormal); }

    // if tangent == (0, 0, 0, 0) then the model has no tangent attribute
    if (vTangent == 0) {
        vec3 normal = vec3(texture(uNormalMap, vUv)) * 2.0 - 1.0;// normalize from [0,1] to [-1,1]
        return normalize(normal) * uNormalScale;
    }

    vec3 T = normalize(vTangent.xyz);
    vec3 N = normalize(vNormal);
    vec3 B = vTangent.w * cross(N, T);
    mat3 TBN = mat3(T, B, N);

    // TODO: should we normalize the normals here???
    vec3 normalMap = texture(uNormalMap, vUv).rgb * 2.0 - 1.0;
    vec3 worldNormal = normalize(TBN * normalMap);
    return worldNormal;
}

// Calculates the fraction of light that reflects vs refracts at a surface depending on the view angle
// aka tells us how shiny a surface looks
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


// Code adapted from https://learnopengl.com/PBR/Lighting
void main()
{
    vec3 N = getNormal();// normal
    vec3 V = normalize(uCameraPos - vPosition);// view direction, position to camera

    // also called albedo
    vec4 baseColor = (((HAS_BASE_COLOR_MAP & uMaterialFlags) != 0u) ? texture(uBaseColorMap, vUv) : vec4(1)) * uBaseColorFactor;
    vec4 occlusionMetallicRoughness = ((HAS_METALLIC_ROUGHNESS_MAP & uMaterialFlags) != 0u) ? texture(uMetallicRoughnessMap, vUv) : vec4(uOcclusionStrength, uMetallicFactor, uRoughnessFactor, 0);

    float ambientOclussion = occlusionMetallicRoughness.r;
    float roughness = occlusionMetallicRoughness.g;
    float metallic = occlusionMetallicRoughness.b;

    vec3 F0 = vec3(0.04);// assume this constant as it looks good for most materials
    F0 = mix(F0, baseColor.rgb, metallic);

    vec3 Lo = vec3(0);
    for (int i = 0; i < lightCount; i++) {
        PointLight light = pointLights[i];
        vec3 L = normalize(light.position - vPosition);// light position to render point world space
        vec3 H = normalize(V + L);// halway vector between view dir and light dir

        float distance = length(light.position - vPosition);
        float attenuation = 1.0 / (distance * distance);// models how light weakens over distance
        vec3 radiance = light.color * attenuation;// the radiance aka intensity and color for point light at this fragment position

        float NDF = distributionGGX(N, H, roughness);
        float G   = geometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
        vec3 specular     = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * baseColor.rgb / PI + specular)* radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * baseColor.rgb * ambientOclussion;
    vec3 color   = ambient + Lo;
    color = color / (color + vec3(1));
    color = pow(color, vec3(1/2.2));

    fragColor = vec4(color, 1);
}