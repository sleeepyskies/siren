#version 460 core

layout (location = 0) in vec3 a_position;

uniform mat4 u_projectionView;
uniform samplerCube u_skybox;

out vec3 v_position;

void main()
{
    v_position = a_position;
    gl_Position = u_projectionView * vec4(a_position, 1.0);
    gl_Position = gl_Position.xyww;// make furthest depth, AHHH
}
