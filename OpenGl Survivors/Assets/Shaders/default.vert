#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

layout (location = 0) uniform mat4 uProjection;

layout (location = 10) uniform mat4 uModel; 

void main()
{
    gl_Position = uProjection * uModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}