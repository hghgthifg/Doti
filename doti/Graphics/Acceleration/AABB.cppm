export module Graphics.Render.Acceleration.AABB;

import std;
import Core.Math;

export class AABB {
public:
    AABB() : _min(0, 0, 0), _max(0, 0, 0) {}
    // AABB(const Vec3& p1, const Vec3& p2) : _min(Core.Math::min(p1, p2)), _max(Core.Math::max(p1, p2)) {}
    AABB(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
        _min = Math::min(v1, v2, v3);
        _max = Math::max(v1, v2, v3);
    }

    AABB(const AABB& a, const AABB& b) {
        _min = Math::min(a._min, b._min);
        _max = Math::max(a._max, b._max);
    }

    auto getCentroid() const -> Vec3 { return (_min + _max) * 0.5f; }

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

    auto maxExtent() const -> int8_t {
        Vec3  v    = _max - _min;
        float maxv = Math::max(v.x, v.y, v.z);
        if (v.x == maxv) return 0;
        if (v.y == maxv) return 1;
        if (v.z == maxv) return 2;
        return -1;
    }

    auto getMin() const -> Vec3 { return _min; }

    auto getMax() const -> Vec3 { return _max; }
    auto setMin(const Vec3& pMin) -> void { _min = pMin; }
    auto setMax(const Vec3& pMax) -> void { _max = pMax; }

    auto mergeWith(const AABB& other) {
        _min = Math::min(_min, other._min);
        _max = Math::max(_max, other._max);
    }

private:
    Vec3 _min, _max;
};
