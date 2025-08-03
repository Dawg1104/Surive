#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 camMatrix;

out vec3 Normal;  // pass to fragment shader
out vec3 FragPos; // optional: fragment position in world space for lighting

void main()
{
    gl_Position = camMatrix * vec4(aPos, 1.0);
    Normal = aNormal; // pass the normal
    FragPos = aPos;   // if model matrix is identity, aPos is world pos
}