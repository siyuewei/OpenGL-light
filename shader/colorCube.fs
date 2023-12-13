//#version 460 core
//out vec4 FragColor;
//  
//in vec3 Normal;
//in vec3 FragPos;
//
//uniform vec3 objectColor;
//uniform vec3 lightColor;
//uniform vec3 lightPos;
//uniform vec3 viewPos;
//
//void main()
//{
//    //ambient
//    float ambient_coefficient = 0.1;
//    vec3 ambient = lightColor * ambient_coefficient;
//
//    vec3 normal = normalize(Normal);
//    //diffuse
//    float diffuse_coefficient = 1;
//    vec3 lightDir = normalize(lightPos - FragPos);
//    vec3 diffuse = lightColor * max(0,dot(normal,lightDir)) * diffuse_coefficient;
//
//    //specular
//    float specular_coefficient = 0.5;
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflect = reflect(-lightDir,normal);
//    vec3 specular = lightColor * pow(max(0,dot(viewDir,reflect)),32) * specular_coefficient;
//
//    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
//}

//#version 460 core
//out vec4 FragColor;
//
////texture存储不同位置的漫反射，镜面反射系数，按texture顶点位置取出就好
////这里考虑环境光的系数和漫反射一致
////shininess是镜面反射中cos的指数值，决定高光大小
//struct Material {
//	sampler2D diffuse;
//	sampler2D specular;
//	float shininess;
//	sampler2D emission;
//};
//
////存储光照的强度，因为实际上光照在传播过程中会有损失
////也就是I/(R^2)
////但没有明白为什么要对不同分量分开存储，似乎所有分量的光照都是一致的
////以及为什么材质的漫反射和镜面反射系数可以用颜色分量表示（分别表示三个分量的系数？）
//struct Light {
//	vec3 position;
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//};
//
//in vec3 FragPos;
//in vec3 Normal;
//in vec2 TexCoords;
//
//uniform vec3 viewPos;
//uniform Material material;
//uniform Light light;
//
//uniform float emission_move;
//
////问题：两张纹理贴图公用一个纹理顶点
//void main()
//{
//	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
//
//    vec3 normal = normalize(Normal);
//	vec3 lightDir = normalize(light.position - FragPos);
//	vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb * max(0, dot(normal, lightDir));
//
//	vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflect = reflect(-lightDir,normal);
//	vec3 specular = light.specular * texture(material.specular, TexCoords).rgb * pow(max(0,dot(reflect,viewDir)),material.shininess);
//
//	vec3 emission = texture(material.emission,vec2(TexCoords.x,TexCoords.y + emission_move)).rgb;
//	FragColor = vec4(ambient + diffuse + specular + emission, 1.0) ;
//}



#version 460 core
out vec4 FragColor;

//texture存储不同位置的漫反射，镜面反射系数，按texture顶点位置取出就好
//这里考虑环境光的系数和漫反射一致
//shininess是镜面反射中cos的指数值，决定高光大小
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	sampler2D emission;
};

//ambient diffuse specular存储光照的强度，因为实际上光照在传播过程中会有损失，也就是I/(R^2) （个人理解）
//constant linear quadratic是衰减系数
//把光源看作聚光，cutOff outCutOff是光照锥形的内外锥角度的cos值
// 问题：
//1.为什么要对不同分量分开存储，似乎所有分量的光照都是一致的
//2.为什么材质的漫反射和镜面反射系数可以用颜色分量表示（分别表示三个分量的系数？）
//3.既然有衰减系数计算光强，那为什么还要ambient diffuse specular这三个数？
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float constant;
    float linear;
    float quadratic;

	vec3 direction;
	float cutOff;
	float outCutOff;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform float emission_move;

//问题：两张纹理贴图公用一个纹理顶点
void main()
{
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb * max(0, dot(normal, lightDir));

	vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflect = reflect(-lightDir,normal);
	vec3 specular = light.specular * texture(material.specular, TexCoords).rgb * pow(max(0,dot(reflect,viewDir)),material.shininess);

	vec3 emission = texture(material.emission,vec2(TexCoords.x,TexCoords.y + emission_move)).rgb;

	//计算光的衰减
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	//判断光照位置与光源光锥的位置关系
	float cos_theta = dot(normalize(light.direction),normalize(FragPos - light.position));
	float intensity = clamp((cos_theta - light.outCutOff)/(light.cutOff - light.outCutOff),0.0f,1.0f);
	//不对环境光处理，留下一点光照
	diffuse *= intensity;
	specular *= intensity;

	FragColor = vec4((ambient + diffuse + specular + emission) * attenuation, 1.0) ;
}