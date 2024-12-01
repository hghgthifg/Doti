#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 viewPos;

uint wseed;
float randcore(uint seed) {
    seed = (seed ^ uint(61)) ^ (seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> uint(4));
    seed *= uint(0x27d4eb2d);
    wseed = seed ^ (seed >> uint(15));
    return float(wseed) * (1.0 / 4294967296.0);
}
float rand() {
    return randcore(wseed);
}

void main() {
    wseed = uint(float(69557857) * (TexCoords.x * TexCoords.y));
    if (distance(TexCoords, vec2(0.5, 0.5)) < 0.4) {
        FragColor = vec4(rand(), rand(), rand(), 1.0);
    }
    else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

