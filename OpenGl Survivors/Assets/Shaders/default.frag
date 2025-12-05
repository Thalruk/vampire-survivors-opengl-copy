#version 460 core
out vec4 FragColor;

in vec2 TexCoord;      
uniform sampler2D image; 
layout (location = 11) uniform vec3 uColor;

void main()
{
    vec4 texColor = texture(image, TexCoord);
    FragColor = texColor * vec4(uColor, 1.0);
    
    if(FragColor.a < 0.1)
        discard;
}