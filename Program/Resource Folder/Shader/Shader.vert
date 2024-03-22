#version 460 core
layout (location = 0) in vec2 Position;
//layout (location = 1) in vec3 Color;
layout (location = 1) in vec2 aTexCoord;

//out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(Position, 0.0, 1.0);
    //ourColor = Color;
    TexCoord = aTexCoord;
}