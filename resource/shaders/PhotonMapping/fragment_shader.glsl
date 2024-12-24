#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D historyTexture;
uniform int screenWidth;
uniform int screenHeight;
uniform int frameCount;

int seed = 3;
float arpeture_size = 2.0;
float check_radius = 5.0;
float bri = 0.7;
float gamma = 2.2;
float refl = 0.5;
vec3 rgb;
int frand(void) { seed = seed * 0x343fd + 0x269ec3; return (seed >> 16) & 32767; }
float rand(void) { return float(frand()) / 32767.0; }
vec3 point;
vec3 lightpos;
float hash1(uint n) {
    // integer hash copied from Hugo Elias
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return float(n & uint(0x7fffffffU)) / float(0x7fffffff);
}

float sde;
vec3 pos;
float maxl = 10000.0;

float plane_sde(vec3 ray, vec3 nor, float planey, vec3 col, float ref) {
    if (dot(ray, nor) + planey < sde) {
        sde = dot(ray, nor) + planey;
        rgb = col;
        refl = ref;
    }
    return dot(ray, nor) + planey;
}

float sphere_sde(vec3 ray, vec3 position, float radius, vec3 col, float ref) {
    if (length(position - ray) - radius < sde) {
        sde = length(position - ray) - radius;
        rgb = col;
        refl = ref;
    }
    return length(position - ray) - radius;
}

float calc_sdf(vec3 ray) {
    sde = maxl;
    plane_sde(ray, vec3(0, 1, 0), 10.0, vec3(bri, bri, bri), 1.0);
    //plane_sde(ray,vec3(0,-1,0),100.0,vec3(1,1,1));
    //plane_sde(ray,vec3(1,0,0),40.0,vec3(0,1,0));
    //plane_sde(ray,vec3(-1,0,0),40.0,vec3(1,0,0));
    sphere_sde(ray, vec3(-20.0, 0.0, 80.0), 5.0, vec3(bri, 0, 0), 0.0);
    sphere_sde(ray, vec3(-10.0, -5.0, 80.0), 5.0, vec3(bri, bri, bri), 1.0);
    sphere_sde(ray, vec3(10.0, 2.0, 80.0), 10.0, vec3(0, bri, bri), 0.0);
    sphere_sde(ray, vec3(-20.0, -5.0, 50.0), 5.0, vec3(0, bri, 0), 0.0);
    return sde;
}


vec3 calcNormal(in vec3 p) // for function f(p)
{
    const float eps = 0.0001; // or some other value
    const vec2 h = vec2(eps, 0);
    return normalize(vec3(calc_sdf(p + h.xyy) - calc_sdf(p - h.xyy),
                     calc_sdf(p + h.yxy) - calc_sdf(p - h.yxy),
                     calc_sdf(p + h.yyx) - calc_sdf(p - h.yyx)));
}

vec4 raycol(vec3 orig, vec3 dir, float ml) {
    float len = 0.0;
    sde = ml;
    float steps = 0.0;
    pos = orig;

    while (len < ml && sde > 0.01) {
        sde = calc_sdf(pos);
        pos += dir * sde;
        len += sde;
        steps += 1.0;
    }
    return vec4(pos, steps);
}


vec3 sphrand() {
    point = vec3(2.0);
    while (length(point) >= 1.0) {
        point = vec3(rand() * 2.0 - 1.0, rand() * 2.0 - 1.0, rand() * 2.0 - 1.0);
    }
    return point;
}


void main()
{
    vec3 hist = texture(historyTexture, TexCoords).rgb;
    //vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    vec2 iResolution;
    iResolution.x = screenWidth;
    iResolution.y = screenHeight;
    vec2 newfrag = vec2(TexCoords.x + iResolution.x / 2.0, TexCoords.y + iResolution.y / 2.0);
    uvec2 p = uvec2((newfrag - .5 * iResolution.xy) / iResolution.y * 1000.0);
    seed = int(round(hash1(p.x + 1920U * p.y + (1920U * 1080U)) * 100.0 * float(frameCount)));
    vec2 th = TexCoords / iResolution.xy;
    vec3 colour = vec3(0);
    for (int i = 0; i < 1; i++) {
        vec2 uv = (TexCoords - .5 * iResolution.xy) / iResolution.y + sphrand().xy / iResolution.xy;
        vec3 pos = vec3(sphrand().x * arpeture_size, sphrand().y * arpeture_size, -1.0);
        vec3 direction = normalize(normalize(vec3(uv.x, uv.y, 1.0)) * 80.0 - pos);
        vec4 o;
        float steps;
        vec3 newdir = direction;
        o = raycol(pos, direction, 500.0);
        if (sde <= 0.01) {
            sde = 0.01;
            vec3 tcol = vec3(1.0);
            float bounces = 0.0;
            //newdir = calcNormal(o.xyz);
            while (sde <= 0.01 && bounces < 3.0) {
                steps = o.w;
                vec3 pre = rgb;
                float pref = refl;
                vec3 inter = o.xyz + calcNormal(o.xyz) / 10.0;
                for (int i = 0; i < 10; i++) {
                    vec3 offset = sphrand();
                    offset *= 200.0;
                    lightpos = vec3(-60, 200.0, -100.0) + offset;
                    offset = sphrand();
                    offset *= check_radius;
                    vec3 hit = raycol(lightpos, normalize(o.xyz + offset - lightpos), length(o.xyz + offset - lightpos)).xyz;
                    if (length(hit - o.xyz) < check_radius) {
                        pre += rgb;//*dot(calcNormal(hit),normalize(lightpos-hit))*dot(n,calcNormal(hit))*(1.0-length(hit-inter)/check_radius);
                    }
                }
                if (pref < rand()) {
                    newdir = sphrand();
                    if (dot(newdir, calcNormal(o.xyz)) < 0.0) {
                        newdir *= -1.0;
                    }
                } else {
                    vec3 normal = calcNormal(o.xyz);
                    newdir = newdir - 2.0 * dot(newdir, normal) * normal;
                }
                pre /= 10.0;
                tcol *= pre;
                o = raycol(inter, normalize(newdir), 500.0);
                bounces += 1.0;
            }
            colour += tcol;
        } else {
            colour += 0.7;
        }
    }
    colour /= 1.0;
    //    vec4 data = texture(hist, th);
    //    data += vec4(pow(colour, vec3(1.0 / gamma)), 1);
    //    colour = (1.0 / float(frameCount)) * colour + (float(frameCount - 1) / float(frameCount)) * hist;
    FragColor = vec4(colour, 1.0);
}