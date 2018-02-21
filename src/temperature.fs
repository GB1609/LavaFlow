#version 330 core
out vec4 FragColor;

in vec3 Color;
void main()
{
	float alpha=0.5;
	if(Color.x==0.0f)
	{
		alpha=0.1f;
	}
	else
	{
		alpha=1.0f;
	}
	FragColor = vec4(1.0f,Color.x,0.0f, alpha);
}