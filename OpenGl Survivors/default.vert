#version 460 core
uniform vec2 uMove;
layout (location = 0) in vec3 aPos;
void main()
{
   gl_Position = vec4(aPos.x + uMove.x, aPos.y + uMove.y, aPos.z, 1.0);
}