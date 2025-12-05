#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (location = 0) uniform mat4 uProjection;
layout (location = 10) uniform vec2 uMove;

out vec2 TexCoord;

void main()
{
    gl_Position = uProjection * vec4(aPos.x + uMove.x, aPos.y + uMove.y, aPos.z, 1.0);
    TexCoord = aTexCoord; 
}