#version 410 core

in vec3 coordonateTexturaSkybox;
out vec4 culoarea;

uniform samplerCube skybox;

void main()
{
    culoarea = texture(skybox, coordonateTexturaSkybox);
}
