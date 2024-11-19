/*!
 * @brief Definition of a ray.
 */

export module Utils.Ray;

import Math;

export class Ray {
public:
    Ray() = default;

    Ray(const Point3&& origin, const Vec3&& direction) = delete;

    Ray(const Point3& origin, const Vec3& direction) : orig(origin), dir(direction) {}

    auto origin() const -> const Point3 & {
        return orig;
    }

    auto direction() const -> const Vec3 & {
        return dir;
    }

    auto at(double t) const -> Point3 {
        return orig + t * dir;
    }

private:
    Point3 orig;
    Vec3   dir;
};
