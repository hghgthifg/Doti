export module Graphics.Render.Acceleration.Primitive.Sphere;

import Graphics.Render.Acceleration.PrimitiveBase;

export class SpherePrimitive : public PrimitiveBase {
public:
    SpherePrimitive(const Vec3& center, float radius) : PrimitiveBase(PrimitiveType::Sphere) {
        const Vec3 a = center - Vec3(radius, radius, radius);
        const Vec3 b = center + Vec3(radius, radius, radius);
        _bounding    = AABB(a, b);
        _centroid    = center;
    }
};
