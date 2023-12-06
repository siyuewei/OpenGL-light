#version 460 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	sampler2D emission;
};

struct DirLight {
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform float emission_move;


void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0f,0.0f,0.0f);
    result += CalcDirLight(dirLight, normal, viewDir);
    for(unsigned int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
    }
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir);


    //加入自发光会影响光照的观察，因为太亮了。乘0.01基本看不见自发光了。
	vec3 emission = texture(material.emission,vec2(TexCoords.x,TexCoords.y + emission_move)).rgb;
    result += emission * 0.01;

    FragColor = vec4(result ,1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    //一定要记得标准化向量，不然会很抽象!!!
    vec3 reflectDir = normalize(reflect(-light.direction, normal));

    vec3 ambient = light.ambient * texture(material.diffuse,TexCoords).rgb;
    vec3 diffuse = light.diffuse * texture(material.diffuse,TexCoords).rgb * max(0,dot(normal, -normalize(light.direction)));
	vec3 specular = light.specular * texture(material.specular, TexCoords).rgb * pow(max(0,dot(reflectDir,viewDir)),material.shininess);
    
    return (ambient + diffuse  + specular );
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 reflectDir = normalize(reflect(light.position - fragPos, normal));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords)).rgb;
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse,TexCoords)).rgb * max(0,dot(normal, normalize(light.position - fragPos)));
    vec3 specular = light.specular * vec3(texture(material.specular, TexCoords)).rgb * pow(max(0,dot(viewDir,reflectDir)),material.shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
    
    return (ambient + diffuse + specular ) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 reflectDir = normalize(reflect(light.position - fragPos, normal));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords)).rgb;
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse,TexCoords)).rgb * max(0,dot(normal, normalize(light.position - fragPos)));
    vec3 specular = light.specular * vec3(texture(material.specular, TexCoords)).rgb * pow(max(0,dot(viewDir,reflectDir)),material.shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
    
    //判断光照位置与光源光锥的位置关系
	float cos_theta = dot(normalize(light.direction),normalize(FragPos - light.position));
	float intensity = clamp((cos_theta - light.outerCutOff)/(light.cutOff - light.outerCutOff),0.0f,1.0f);
	//不对环境光处理，留下一点光照
	diffuse *= intensity;
	specular *= intensity;

    return (ambient + diffuse + specular ) * attenuation;
}