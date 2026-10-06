/*
gridFloor that will act as the base of the arm. 
Objects have contact.
*/

#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 gridColor;
uniform float gridSpacing;
uniform float lineWidth;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec2 coord = FragPos.xz / gridSpacing; // only xz plane
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    float line = min(grid.x, grid.y);
    float intensity = 1.0 - min(line, 1.0);

    if (intensity < 0.1) discard; // transparency


    float ambientStrength = 0.5f;
    float specularStrength = 0.1f;

    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(reflectDirection, viewDirection), 0.0), 32.0);
    vec3 specular = spec * specularStrength * lightColor;

    vec3 result = (diffuse + ambient) * gridColor + specular;

    FragColor = vec4(result, intensity);
}