#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    TexCoords = aPos;
    mat4 view_without_trans = mat4(mat3(view));
    vec4 pos = projection * view_without_trans * vec4(aPos, 1.0);
    //����պе�z��������Ϊ1��Ҳ����������ֵ����֤��պ���������Ⱦ����ĺ���
    gl_Position = pos.xyww;
}  