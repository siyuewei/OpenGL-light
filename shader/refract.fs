#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D texture_diffuse1;

void main()
{    
//    float ratio = 1.00 / 1.52;
//    vec3 I = normalize(Position - cameraPos);
//    vec3 R = refract(I, normalize(Normal), ratio);
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);

    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    vec3 refractColor = texture(skybox, R).rgb;

    vec3 diffuseColor = texture(texture_diffuse1, TexCoords).rgb;

    FragColor = vec4((refractColor * 0.2 + diffuseColor *0.8 ), 1.0);
}