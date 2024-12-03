#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

struct Camera {
    vec3 position;
    vec3 front;
    vec3 right;
    vec3 up;
    float halfHeight;
    float halfWidth;
    vec3 leftBottom;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Sphere {
    vec3 center;
    vec3 albedo;
    float radius;
    int materialIndex;
};

struct hitRecord {
    vec3 normal;
    vec3 position;
    vec3 albedo;
    int materialIndex;
};

uniform sampler2D historyTexture;
uniform int screenWidth;
uniform int screenHeight;
uniform int frameCount;
uniform float randOrigin;

uniform Camera camera;
uniform Sphere sphere[4];

hitRecord rec;
uint wseed;

float randcore(uint seed);
float rand(void);
float hitSphere(Sphere s, Ray r);
bool hitWorld(Ray r);

float randcore(uint seed) {
    seed = (seed ^ uint(61)) ^ (seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> uint(4));
    seed *= uint(0x27d4eb2d);
    wseed = seed ^ (seed >> uint(15));
    return float(wseed) * (1.0 / 4294967296.0);
}

// Get a random float
float rand() {
    return randcore(wseed);
}

// Get the distance between a sphere and a ray's origin
float hitSphere(Sphere s, Ray r) {
    vec3 oc = r.origin - s.center;
    float a = dot(r.direction, r.direction);
    float b = 2.0 * dot(oc, r.direction);
    float c = dot(oc, oc) - s.radius * s.radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant > 0.0) {
        float dis = (-b - sqrt(discriminant)) / (2.0 * a);
        if (dis > 0.0) return dis;
        else return -1.0;
    } else {
        return -1.0;
    }
}

// Check if the ray hit anything
bool hitWorld(Ray r) {
    float dis = 100000;
    bool hitAnything = false;
    int hitSphereIndex;
    for (int i = 0; i < 4; i++) {
        float tempDis = hitSphere(sphere[i], r);
        if (tempDis > 0.0 && tempDis < dis) {
            dis = tempDis;
            hitAnything = true;
            hitSphereIndex = i;
        }
    }
    if (hitAnything) {
        rec.position = r.origin + dis * r.direction;
        rec.normal = normalize(r.origin + dis * r.direction - sphere[hitSphereIndex].center);
        rec.albedo = sphere[hitSphereIndex].albedo;
        rec.materialIndex = sphere[hitSphereIndex].materialIndex;
        return true;
    }
    return false;
}

// Get a random vecter in unit sphere
vec3 randomInUnitSphere() {
    vec3 p;
    do {
        p = 2.0 * vec3(rand(), rand(), rand()) - vec3(1.0, 1.0, 1.0);
    } while (dot(p, p) >= 1.0);
    return p;
}

vec3 diffuseReflection(vec3 normal) {
    return normalize(normal + randomInUnitSphere());
}

vec3 metalReflection(vec3 normal, vec3 rayIn) {
    return normalize(rayIn - 2 * dot(rayIn, normal) * normal + 0.35 * randomInUnitSphere());
}

vec3 shading(Ray r) {
    vec3 color = vec3(1.0, 1.0, 1.0);
    bool hitAnything = false;
    for (int i = 0; i < 20; i++) {
        if (hitWorld(r)) {
            r.origin = rec.position;
            if (rec.materialIndex == 0)
            r.direction = diffuseReflection(rec.normal);
            else if (rec.materialIndex == 1)
            r.direction = metalReflection(rec.normal, r.direction);
            color *= rec.albedo;
            hitAnything = true;
        } else {
            break;
        }
    }
    if (!hitAnything) color = vec3(0.0, 0.0, 0.0);
    return color;
}

void main() {
    wseed = uint(randOrigin * float(6.95857) * (TexCoords.x * TexCoords.y));
    vec3 hist = texture(historyTexture, TexCoords).rgb;

    Ray cameraRay;
    cameraRay.origin = camera.position;
    cameraRay.direction = normalize(camera.leftBottom + (TexCoords.x * 2.0 * camera.halfWidth) * camera.right + (TexCoords.y * 2.0 * camera.halfHeight) * camera.up);

    vec3 curColor = shading(cameraRay);
    curColor = (1.0 / float(frameCount)) * curColor + (float(frameCount - 1) / float(frameCount)) * hist;
    FragColor = vec4(curColor, 1.0);
}