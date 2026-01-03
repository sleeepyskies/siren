#version 460 core

// ==================================
// Interpolated Inputs
// ==================================
in vec3 v_position;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec2 v_uv;

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
uniform mat4 u_model;


// ==================================
// Outputs
// ==================================
out vec4 fragColor;

// ==================================
// Constants
// ==================================
const float CELL_LINE_WIDTH = 0.02;
const float SUB_CELL_LINE_WIDTH = 0.002;

const vec2 CELL_SIZE = vec2(10);
const vec2 SUB_CELL_SIZE = vec2(1);

const vec2 HALF_CELL_SIZE = CELL_SIZE * 0.5;
const vec2 HALF_SUB_CELL_SIZE = SUB_CELL_SIZE * 0.5;

const vec4 CELL_COLOR = vec4(0.75, 0.75, 0.75, 0.5);
const vec4 SUB_CELL_COLOR = vec4(0.5, 0.5, 0.5, 0.5);

const vec4 RED = vec4(0.7, 0.25, 0.25, 1);
const vec4 GREEN = vec4(0.05, 0.7, 0.05, 1);
const vec4 BLUE = vec4(0.25, 0.4, 0.7, 1);

const float MIN_FADE_DIST = CELL_SIZE.x * 2;
const float MAX_FADE_DIST = CELL_SIZE.x * 20;

// https://dev.to/javiersalcedopuyo/simple-infinite-grid-shader-5fah
void main()
{
    vec2 cellCoords = mod(v_position.xz + HALF_CELL_SIZE, CELL_SIZE);
    vec2 subCellCoords = mod(v_position.xz + HALF_SUB_CELL_SIZE, SUB_CELL_SIZE);
    float distanceToCell = min(abs(cellCoords.x - HALF_CELL_SIZE.x), abs(cellCoords.y - HALF_CELL_SIZE.y));
    float distanceToSubCell = min(abs(subCellCoords.x - HALF_SUB_CELL_SIZE.x), abs(subCellCoords.y - HALF_SUB_CELL_SIZE.y));

    float derivativeCell = fwidth(distanceToCell);
    float derivativeSubCell = fwidth(distanceToSubCell);
    float adjustedCellWidth = (CELL_LINE_WIDTH + derivativeCell);
    float adjustedSubCellWidth = (SUB_CELL_LINE_WIDTH + derivativeSubCell);

    vec4 color;

    if (distanceToSubCell < adjustedSubCellWidth * 0.5) {
        color = SUB_CELL_COLOR;
    } else if (distanceToCell < adjustedCellWidth * 0.5) {
        color = CELL_COLOR;
    } else {
        discard;
    }

    if (abs(v_position.x) < adjustedCellWidth) {
        color = RED;
    }

    if (abs(v_position.z) < adjustedCellWidth) {
        color = BLUE;
    }

    float distToCamera = distance(v_position, cameraPosition);
    float fadeDistance = abs(cameraPosition.y) * MAX_FADE_DIST;
    fadeDistance = clamp(fadeDistance, MIN_FADE_DIST, MAX_FADE_DIST);
    float opacityFalloff = clamp(1.0 - distToCamera / fadeDistance, 0.0, 1.0);

    fragColor = vec4(color.rgb, color.a * opacityFalloff);
}
