#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxTextureSampler;

void main()
{    
    FragColor = texture(skyboxTextureSampler, TexCoords);
}