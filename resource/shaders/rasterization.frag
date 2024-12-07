#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
