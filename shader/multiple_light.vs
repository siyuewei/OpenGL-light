#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model = mat4(1.0f);
//uniform mat4 view = mat4(1.0f);
//uniform mat4 projection = mat4(1.0f);
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	//使用世界坐标进行光照计算,把法向量和点的位置都转换到世界坐标
	Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * vec4(aPos,1.0));
	TexCoords = aTexCoords;
}