#version 410 core

layout (location = 0) in vec3 coordVarf;
out vec3 coordonateTexturaSkybox;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pozitieAux = projection * view * vec4(coordVarf, 1.0);
    gl_Position = pozitieAux.xyww;
    coordonateTexturaSkybox = coordVarf;
}
