#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout (std430, binding = 0) buffer IndexBuffer {
    uvec3 indices[];
    float _index_padding;
};

layout (std430, binding = 1) buffer VertexBuffer {
    vec3 vertices[];
    float _vertex_padding;
};

layout (std430, binding = 2) buffer NormalBuffer {
    vec3 normals[];
    float _normal_padding;
};

struct AABB {
    vec4 pMin;
    vec4 pMax;
};

struct BVHNode {
    AABB box;
    int index; // max size of the tree if > 0, primitive index if < 0
    float _bvh_padding[3];
};

layout (std430, binding = 3) buffer BVHTree {
    BVHNode nodes[];
};

struct Triangle {
    vec3 v0, v1, v2;
};

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

struct hitRecord {
    vec3 normal;
    vec3 position;
    vec3 albedo;
};

uniform sampler2D historyTexture;
uniform int screenWidth;
uniform int screenHeight;
uniform int frameCount;
uniform float randOrigin;

uniform Camera camera;
uint wseed;

float randcore(uint seed);
float rand(void);
float at(sampler2D dataTex, float index);
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

// Get the distance between a triangle and a ray's origin
float distanceToTri(Ray r, Triangle tri) {
    vec3 vv = vec3(0.0, 0.0, 0.0);
    if (tri.v0 == vv && tri.v1 == vv && tri.v2 == vv) return 0.0;

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

bool intersectAABB(Ray ray, AABB box) {
    float epsilon = 1e-8;
    vec3 invDir = sign(ray.direction) / max(abs(ray.direction), vec3(epsilon));
    vec3 t0 = (box.pMin.xyz - ray.origin) * invDir;
    vec3 t1 = (box.pMax.xyz - ray.origin) * invDir;
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);
    float tEnter = max(max(tmin.x, tmin.y), tmin.z);
    float tExit = min(min(tmax.x, tmax.y), tmax.z);
    return tEnter <= tExit && tExit >= 0.0;
}

bool hitWorld(Ray r, out hitRecord rec) {
    int stack[64];
    int stackPtr = 0;
    stack[stackPtr++] = 0;

    float minDis = 100000;

    bool hitAnything = false;
    while (stackPtr > 0) {
        int nodeIndex = stack[--stackPtr];
        BVHNode node = nodes[nodeIndex];

        if (intersectAABB(r, node.box)) {
            if (node.index < 0) {
                int hybridIndex = -node.index;
                uint priType = (hybridIndex & 0xf0000000) >> 28;
                uint priIndex = hybridIndex & 0x0fffffff;
                if (priType == 1) {
                    // Triangle
                    Triangle tri;
                    tri.v0 = vertices[indices[priIndex].x];
                    tri.v1 = vertices[indices[priIndex].y];
                    tri.v2 = vertices[indices[priIndex].z];

                    float dis = distanceToTri(r, tri);
                    if (dis > 0.0) {
                        hitAnything = true;
                        if (dis < minDis) {
                            minDis = dis;
                            rec.normal = normals[priIndex];
                            rec.albedo = vec3(0.17, 0.17, 0.87);
                            rec.position = r.origin + dis * r.direction;
                        }
                    }
                } else if (priType == 2) {
                    // Sphere
                } else {
                    rec.albedo = vec3(1.0, 1.0, 1.0);
                    //                    if (priType == 0)
                    //                    rec.albedo = vec3(1.0, 0.1, 0.1);
                    //                    else
                    //                    rec.albedo = vec3(nodeIndex / 10.f, priType / 10.f, 1.0);
                    return true;
                }
            } else {
                int size = nodes[nodeIndex].index;
                int lsize = nodes[nodeIndex + 1].index;
                stack[stackPtr++] = nodeIndex + 1; // Left child
                if (lsize < 0) lsize = 1;
                stack[stackPtr++] = nodeIndex + lsize + 1;
            }
        }
    }
    return hitAnything;
}

vec3 shading(Ray r) {
    vec3 color = vec3(1.0, 1.0, 1.0);
    bool hitAnything = false;
    for (int i = 0; i < 20; i++) {
        hitRecord rec;
        if (hitWorld(r, rec)) {
            r.origin = rec.position;
            r.direction = diffuseReflection(rec.normal);
            // if (rec.materialIndex == 0)
            // r.direction = diffuseReflection(rec.normal);
            // else if (rec.materialIndex == 1)
            // r.direction = metalReflection(rec.normal, r.direction);
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