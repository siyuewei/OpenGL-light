#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
	vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // bottom left
	EmitVertex();
	gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0); // bottom right
	EmitVertex();
	gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0); // top left
	EmitVertex();
	gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0); // top right
	EmitVertex();
	gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0); // top
	EmitVertex();
	EndPrimitive();
}

void main() {
	for(int i = 0; i < gl_in.length(); i++) {
		fColor = gs_in[i].color;
		build_house(gl_in[i].gl_Position);
	}
}