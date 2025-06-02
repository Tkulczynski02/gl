#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos_world; // Pozycja fragmentu w przestrzeni świata
out vec3 Normal_world;  // Normalna w przestrzeni świata
out vec2 TexCoords_pass; // Przekazane współrzędne tekstury

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos_world = vec3(model * vec4(aPos, 1.0));
    Normal_world = mat3(transpose(inverse(model))) * aNormal;
    TexCoords_pass = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}