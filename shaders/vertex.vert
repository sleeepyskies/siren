#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTextureUV;

out vec3 position;
out vec3 color;
out vec3 normal;
out vec2 texUV;

uniform mat4 model;
uniform mat4 projView;
uniform sampler2D tex0;

void main()
{
    // matrix multiplication is right to left
    position = vec3(model * vec4(aPosition, 1.f));
    gl_Position = projView * vec4(position, 1.f);
    color = aColor;
    normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    texUV = aTextureUV;
}
