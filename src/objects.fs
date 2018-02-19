#version 330 core
out vec4 FragColor;

in vec3 Normal;
void main()
{
	FragColor = vec4(Normal, 1.0f);
	//FragColor = vec4(1.0,1.0,1.0, 1.0f);
}