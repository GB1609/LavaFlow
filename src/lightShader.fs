 #version 330 core

in vec3 FragPos; 
in vec3 Normal;
in vec2 TextCoords;  
in float tempColor;
out vec4 finalColor;


uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform sampler2D TEXTURE;

void main()
{
	vec3 colorTexture = texture(TEXTURE, TextCoords).rgb;
	vec3 colorTemperature;
    if(tempColor==0.0f)
    {
    	colorTemperature=vec3(1.0f,1.0f,1.0f); //in modo tale da non effettuare cambiamenti se non ce lava
    }
    else    
    {
    	colorTemperature=vec3(1.0f,tempColor,0.0f);// graduazione di giallo
    }


	vec3 lightColor=vec3(1.0f,1.0f,1.0f);
	
    float constantAmbient = 0.7f;
    vec3 ambient = constantAmbient * lightColor *colorTexture *colorTemperature;//*colorTexture
  	
  	
  	
    // Diffuse 
    vec3 normale = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normale, lightDir), 0.0);
    if(colorTexture.x==0.0f &&colorTexture.y==0.0f &&colorTexture.z==0.0f)
    {
    	colorTexture.x=0.7f;
    	colorTexture.y=0.7f;
    	colorTexture.z=0.7f;
    }
    vec3 diffuse = diff * lightColor*colorTexture*colorTemperature;
    
    
   // specular
   float specularIntensity = 0.1f;
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 halfwayDir = normalize(lightDir + viewDir);  
   float spec = pow(max(dot(normale, halfwayDir), 0.0), 32); 
   vec3 specular = specularIntensity * spec * lightColor*colorTemperature;
     
    vec3 result = (ambient + diffuse + specular);
   	
   	finalColor = vec4(result, 1.0f);
}