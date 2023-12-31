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
    //把天空盒的z分量设置为1，也就是最大深度值，保证天空盒在所有渲染物体的后面
    gl_Position = pos.xyww;
}  