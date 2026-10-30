#version 330 core

in vec3 dColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(dColor.rgb, 1.0);
}
