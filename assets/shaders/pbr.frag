#version 460 core

// ==================================
// Interpolated Inputs
// ==================================
in vec3 v_position;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec2 v_uv;
in vec4 v_color;

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

layout (std140, binding = 2) uniform Environment {
    bool hasSkyBox;
};

uniform samplerCube u_skybox;

// ==================================
// Required Uniforms
// ==================================
uniform mat4 u_model;

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
const uint HAS_SKY_BOX                 = 1u << 6;

// ==================================
// Outputs
// ==================================
out vec4 fragColor;

const float PI = 3.14159265359;

vec3 getNormal() {
    if ((HAS_NORMAL_MAP & u_materialFlags) == 0u) { return normalize(v_normal); }

    vec3 T = normalize(v_tangent);
    vec3 N = normalize(v_normal);
    vec3 B =  normalize(v_bitangent);
    mat3 TBN = mat3(T, B, N);

    // TODO: should we normalize the normals here???
    vec3 normalMap = texture(u_normalMap, v_uv).rgb * 2.0 - 1.0;
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
    vec3 V = normalize(cameraPosition- v_position);// view direction, position to camera

    // also called albedo
    vec4 baseColor = (((HAS_BASE_COLOR_MAP & u_materialFlags) != 0u) ? texture(u_baseColorMap, v_uv) : v_color) * u_baseColorFactor;
    float metallic = ((HAS_METALLIC_ROUGHNESS_MAP & u_materialFlags) != 0u) ? texture(u_metallicRoughnessMap, v_uv).r : u_metallicFactor;
    float roughness = ((HAS_METALLIC_ROUGHNESS_MAP & u_materialFlags) != 0u) ? texture(u_metallicRoughnessMap, v_uv).g : u_roughnessFactor;
    float ambientOclusion = ((HAS_OCCLUSION_MAP & u_materialFlags) != 0u) ? texture(u_occlusionMap, v_uv).r : u_occlusionStrength;

    vec3 F0 = vec3(0.04);// assume this constant as it looks good for most materials
    F0 = mix(F0, baseColor.rgb, metallic);

    vec3 Lo = vec3(0);
    for (int i = 0; i < pointLightCount; i++) {
        PointLight light = pointLights[i];
        vec3 L = normalize(light.position - v_position);// light position to render point world space
        vec3 H = normalize(V + L);// halway vector between view dir and light dir

        float distance = length(light.position - v_position);
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

    vec3 ambient = vec3(0.03) * baseColor.rgb * ambientOclusion;
    vec3 color   = ambient + Lo;
    color = color / (color + vec3(1));
    color = pow(color, vec3(1/2.2));

    fragColor = vec4(color, 1);
}
