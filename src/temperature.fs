#version 330 core
out vec4 FragColor;

in vec3 Color;
void main()
{
	if(Color.x==0.0f)
	{
		FragColor=vec4(0.0f,0.0f,0.0f,0.0f);
	}
	else
	{
		FragColor = vec4(Color, 1.0f);
	}
}