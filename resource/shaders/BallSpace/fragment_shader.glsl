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

struct Triangle {
    vec3 v0, v1, v2;
    vec3 n0, n1, n2;
    vec2 u0, u1, u2;
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
uniform Triangle tri[2];

hitRecord rec;
uint wseed;

float randcore(uint seed);
float rand(void);
float at(sampler2D dataTex, float index);
float hitSphere(Sphere s, Ray r);
float hitTriangle(Triangle tri, Ray r);
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

// Get the distance between a triangle and a ray's origin
float hitTriangle(Triangle tri, Ray r) {
    vec3 vv = vec3(0.0, 0.0, 0.0);
    if (tri.v0 == vv && tri.v1 == vv && tri.v2 == vv) return 0.2;

    vec3 A = tri.v1 - tri.v0;
    vec3 B = tri.v2 - tri.v0;
    vec3 N = normalize(cross(A, B));

    if (dot(N, r.direction) == 0) return -1.0;
    float D = -dot(N, tri.v0);
    float t = -(dot(N, r.origin) + D) / dot(N, r.direction);
    if (t < 0) return -1.0;

    vec3 pHit = r.origin + t * r.direction;
    vec3 edge0 = tri.v1 - tri.v0;
    vec3 C0 = pHit - tri.v0;
    if (dot(N, cross(edge0, C0)) < 0) return -1.0;
    vec3 edge1 = tri.v2 - tri.v1;
    vec3 C1 = pHit - tri.v1;
    if (dot(N, cross(edge1, C1)) < 0) return -1.0;
    vec3 edge2 = tri.v0 - tri.v2;
    vec3 C2 = pHit - tri.v2;
    if (dot(N, cross(edge2, C2)) < 0) return -1.0;

    return t - 0.00001;
}

float at(sampler2D dataTex, float index) {
    float row = (index + 0.5) / textureSize(dataTex, 0).x;
    float y = (int(row) + 0.5) / textureSize(dataTex, 0).y;
    float x = (index + 0.5 - int(row) * textureSize(dataTex, 0).x) / textureSize(dataTex, 0).x;

    vec2 texCoord = vec2(x, y);
    return texture2D(dataTex, texCoord).x;
}

//Triangle getTriangle(int index) {
//    Triangle tri_t;
//    float face0Index = at(texMeshFaceIndex, float(index * 3));
//    float face1Index = at(texMeshFaceIndex, float(index * 3 + 1));
//    float face2Index = at(texMeshFaceIndex, float(index * 3 + 2));
//
//    tri_t.v0.x = at(texMeshVertex, face0Index * 8.0);
//    tri_t.v0.y = at(texMeshVertex, face0Index * 8.0 + 1.0);
//    tri_t.v0.z = at(texMeshVertex, face0Index * 8.0 + 2.0);
//
//    tri_t.v1.x = at(texMeshVertex, face1Index * 8.0);
//    tri_t.v1.y = at(texMeshVertex, face1Index * 8.0 + 1.0);
//    tri_t.v1.z = at(texMeshVertex, face1Index * 8.0 + 2.0);
//
//    tri_t.v2.x = at(texMeshVertex, face2Index * 8.0);
//    tri_t.v2.y = at(texMeshVertex, face2Index * 8.0 + 1.0);
//    tri_t.v2.z = at(texMeshVertex, face2Index * 8.0 + 2.0);
//
//    return tri_t;
//}

vec3 getTriangleNormal(Triangle tri) {
    return normalize(cross(tri.v2 - tri.v0, tri.v1 - tri.v0));
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

// Check if the ray hit anything
bool hitWorld(Ray r) {
    float dis = 100000;
    bool ifHitSphere = false;
    bool ifHitTriangle = false;
    int hitSphereIndex;
    int hitTriangleIndex;

    for (int i = 0; i < 4; i++) {
        float tempDis = hitSphere(sphere[i], r);
        if (tempDis > 0.0 && tempDis < dis) {
            dis = tempDis;
            ifHitSphere = true;
            hitSphereIndex = i;
        }
    }

    for (int i = 0; i < 2; i++) {
        float tempDis = hitTriangle(tri[i], r);
        if (tempDis > 0.0 && tempDis < dis) {
            dis = tempDis;
            ifHitTriangle = true;
            hitTriangleIndex = i;
        }
    }

    //    for (int i = 0; i < meshIndexNum / 3; i++) {
    //        float tempDis = hitTriangle(getTriangle(i), r);
    //        if (tempDis > 0 && tempDis < dis) {
    //            dis = tempDis;
    //            hitTriangleIndex = i;
    //            ifHitTriangle = true;
    //        }
    //    }

    if (ifHitTriangle) {
        rec.position = r.origin + dis * r.direction;
        rec.normal = getTriangleNormal(tri[hitTriangleIndex]);
        rec.albedo = vec3(0.17, 0.17, 0.82);
        rec.materialIndex = 1;
        return true;
    } else if (ifHitSphere) {
        rec.position = r.origin + dis * r.direction;
        rec.normal = normalize(r.origin + dis * r.direction - sphere[hitSphereIndex].center);
        rec.albedo = sphere[hitSphereIndex].albedo;
        rec.materialIndex = sphere[hitSphereIndex].materialIndex;
        return true;
    }

    return false;
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