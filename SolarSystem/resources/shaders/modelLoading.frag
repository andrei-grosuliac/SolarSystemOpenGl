
#version 330 core

#define NUMBER_OF_POINT_LIGHTS 2

in vec2 TexCoords;

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform Light lights[NUMBER_OF_POINT_LIGHTS];
vec3 CalcPointLight( Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0.0,0.0,0.0);
    for(int i =0; i<NUMBER_OF_POINT_LIGHTS; i++){
		result += CalcPointLight(lights[i], norm, FragPos, viewDir);
	}
	color = vec4(result, 1.0);
}

vec3 CalcPointLight( Light light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse, TexCoords));
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse, TexCoords));
    
    // Specular    
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = light.specular * spec * vec3(texture( texture_specular, TexCoords));
    
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
	vec3 result = vec3(0.0,0.0,0.0);
    result = vec3(ambient + diffuse + specular);
	return result;
}
