#version 460 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(1.0, -1.0, -1.0)); // example directional light
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(1.0, 1.0, 1.0); // red

void main()
{
    // Normalize interpolated normal
    vec3 norm = normalize(Normal);

    // Simple diffuse lighting (Lambert)
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = diffuse * objectColor;

    FragColor = vec4(result, 1.0);
}