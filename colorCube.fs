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
////texture�洢��ͬλ�õ������䣬���淴��ϵ������texture����λ��ȡ���ͺ�
////���￼�ǻ������ϵ����������һ��
////shininess�Ǿ��淴����cos��ָ��ֵ�������߹��С
//struct Material {
//	sampler2D diffuse;
//	sampler2D specular;
//	float shininess;
//	sampler2D emission;
//};
//
////�洢���յ�ǿ�ȣ���Ϊʵ���Ϲ����ڴ��������л�����ʧ
////Ҳ����I/(R^2)
////��û������ΪʲôҪ�Բ�ͬ�����ֿ��洢���ƺ����з����Ĺ��ն���һ�µ�
////�Լ�Ϊʲô���ʵ�������;��淴��ϵ����������ɫ������ʾ���ֱ��ʾ����������ϵ������
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
////���⣺����������ͼ����һ��������
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

//texture�洢��ͬλ�õ������䣬���淴��ϵ������texture����λ��ȡ���ͺ�
//���￼�ǻ������ϵ����������һ��
//shininess�Ǿ��淴����cos��ָ��ֵ�������߹��С
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	sampler2D emission;
};

//ambient diffuse specular�洢���յ�ǿ�ȣ���Ϊʵ���Ϲ����ڴ��������л�����ʧ��Ҳ����I/(R^2) ��������⣩
//constant linear quadratic��˥��ϵ��
//�ѹ�Դ�����۹⣬cutOff outCutOff�ǹ���׶�ε�����׶�Ƕȵ�cosֵ
// ���⣺
//1.ΪʲôҪ�Բ�ͬ�����ֿ��洢���ƺ����з����Ĺ��ն���һ�µ�
//2.Ϊʲô���ʵ�������;��淴��ϵ����������ɫ������ʾ���ֱ��ʾ����������ϵ������
//3.��Ȼ��˥��ϵ�������ǿ����Ϊʲô��Ҫambient diffuse specular����������
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

//���⣺����������ͼ����һ��������
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

	//������˥��
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	//�жϹ���λ�����Դ��׶��λ�ù�ϵ
	float cos_theta = dot(normalize(light.direction),normalize(FragPos - light.position));
	float intensity = clamp((cos_theta - light.outCutOff)/(light.cutOff - light.outCutOff),0.0f,1.0f);
	//���Ի����⴦������һ�����
	diffuse *= intensity;
	specular *= intensity;

	FragColor = vec4((ambient + diffuse + specular + emission) * attenuation, 1.0) ;
}