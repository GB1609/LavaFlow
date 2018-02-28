#version 330 core
layout (location = 4) in vec3 aPos;
layout (location = 5) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Color;

void main()
{
	Color = vec3(aColor.x,aColor.y,aColor.z); 
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}