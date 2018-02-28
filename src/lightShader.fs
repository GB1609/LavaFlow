 #version 330 core

in vec3 FragPos; 
in vec3 Normal;
in vec2 TextCoords;  
in float tempColor;
out vec4 finalColor;


uniform vec3 lightPos; 
uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform sampler2D TEXTURE;
uniform bool textured;
uniform bool lava;

void main()
{
	vec3 colorTexture;
	if(!textured) //if no texture i set it
	{
		colorTexture.x=0.75f;
    	colorTexture.y=0.75f;
    	colorTexture.z=0.75f;
	}
	else
	{
		colorTexture = texture(TEXTURE, TextCoords).rgb;
	}
	vec3 colorTemperature=vec3(1.0f,tempColor,0.0f);// yellow graduation

	vec3 lightColor=vec3(1.0f,1.0f,1.0f);
	
    float constantAmbient = 0.6f;
    vec3 ambient;
  	
  	
    // Diffuse 
    vec3 normale = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normale, lightDir), 0.0);
    vec3 diffuse;
    
   	// specular
  	float specularIntensity = 0.1f;
   	vec3 viewDir = normalize(viewPos - FragPos);
   	vec3 halfwayDir = normalize(lightDir + viewDir);  
   	float spec = pow(max(dot(normale, halfwayDir), 0.0), 32); 
   	vec3 specular;
   	if(tempColor==0.0f || !lava)
   	{
   		ambient= constantAmbient * lightColor *colorTexture;
   		diffuse = diff * lightColor*colorTexture;
   		specular = specularIntensity * spec * lightColor *colorTexture;
   	}
   	else if(lava)
   	{
   		ambient= constantAmbient * lightColor *colorTemperature;
   		diffuse = diff * lightColor*colorTemperature;
   		specular = specularIntensity * spec * lightColor * colorTemperature;
   	}
     
    vec3 result = (ambient + diffuse + specular);
   	
   	finalColor = vec4(result, 1.0f);
}