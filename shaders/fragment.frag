#version 460 core

in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 texUV;

uniform mat4 model;
uniform mat4 projView;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform sampler2D tex0;

out vec4 fragColor;

void main()
{
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    vec3 viewDir = normalize(camPos - position);
    vec3 reflectionDir = reflect(-lightDir, normal);

    vec4 diffuse = max(dot(normal, lightDir), 0.0) * texture(tex0, texUV) * lightColor;
    vec4 ambient = 0.60 * (texture(tex0, texUV));
    float specularAmt = pow(max(dot(viewDir, reflectionDir), 0.0), 8);
    float specular = specularAmt * 0.5;

    fragColor = diffuse + ambient + specular;
}