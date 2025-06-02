#version 330 core
out vec4 FragColor;

in vec3 FragPos_world;
in vec3 Normal_world;
// in vec2 TexCoords_pass; // Nie używamy TexCoords w tej wersji shadera

struct Light {
    vec3 position; // Pozycja światła w przestrzeni świata
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform vec3 viewPos_world;   // Pozycja kamery/obserwatora w przestrzeni świata
uniform vec3 objectColor;     // Kolor bazowy obiektu przekazywany z C++
uniform float material_shininess; // Współczynnik połyskliwości

void main()
{
    // Ambient
    vec3 ambient = light.ambient * objectColor;

    // Diffuse
    vec3 norm = normalize(Normal_world);
    vec3 lightDir = normalize(light.position - FragPos_world);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * objectColor;

    // Specular
    vec3 viewDir = normalize(viewPos_world - FragPos_world);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    vec3 specular = light.specular * spec * vec3(0.8f); // Stały, jasnoszary kolor odblasków (możesz zmienić)

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}