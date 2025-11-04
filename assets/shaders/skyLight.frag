#version 460 core

// ==================================
// Interpolated Inputs
// ==================================
in vec3 v_position;

uniform mat4 u_projectionView;
uniform samplerCube u_skybox;

// ==================================
// Outputs
// ==================================
out vec4 fragColor;

void main()
{
    fragColor = texture(u_skybox, v_position);
}
