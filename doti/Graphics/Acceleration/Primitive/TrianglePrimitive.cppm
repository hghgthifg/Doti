export module Graphics.Render.Acceleration.Primitive.Triangle;

import Graphics.Render.Acceleration.PrimitiveBase;

export class TrianglePrimitive : public PrimitiveBase {
public:
    TrianglePrimitive(const Vec3& a, const Vec3& b, const Vec3& c) : PrimitiveBase(PrimitiveType::Triangle) {
        _bounding = AABB(Math::min(a, b, c), Math::max(a, b, c));
        _centroid = _bounding.getCentroid();
    }
};
