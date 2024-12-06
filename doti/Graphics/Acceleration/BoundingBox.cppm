export module Graphics.Render.BoundingBox;

import Math;

export class BoundingBox {
public:
    explicit BoundingBox(const Vec3& p1, const Vec3& p2) : _min(Math::min(p1, p2)), _max(Math::max(p1, p2)) {}

    auto getDiagonal() const -> Vec3 { return _max - _min; }

    auto getOffset(const Vec3& p) const -> Vec3 {
        Vec3 o = p - _min;
        if (_max.x > _min.x) o.x /= _max.x - _min.x;
        if (_max.y > _min.y) o.y /= _max.y - _min.y;
        if (_max.z > _min.z) o.z /= _max.z - _min.z;
        return o;
    }

    auto getSurfaceArea() const -> float {
        const Vec3 d = getDiagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    auto getMin() const -> Vec3 { return _min; }
    auto getMax() const -> Vec3 { return _max; }
    auto setMin(const Vec3& pMin) -> void { _min = pMin; }
    auto setMax(const Vec3& pMax) -> void { _max = pMax; }

    auto mergeWith(const BoundingBox& other) {
        _min = Math::min(_min, other._min);
        _max = Math::max(_max, other._max);
    }

    static auto merge(const BoundingBox& a, const BoundingBox& b) -> BoundingBox {
        return BoundingBox(Math::min(a._min, b._min), Math::max(a._max, b._max));
    }

private:
    Vec3 _min, _max;
};
