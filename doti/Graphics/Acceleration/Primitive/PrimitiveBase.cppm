export module Graphics.Render.Acceleration.PrimitiveBase;

export import Core.Math;
export import Graphics.Render.Acceleration.AABB;

export enum class PrimitiveType {
    Triangle,
    Sphere,
    Unknown
};

export class PrimitiveBase {
public:
    explicit PrimitiveBase(const PrimitiveType type) : _type(type) {}

    virtual ~PrimitiveBase() = default;

    virtual auto getAABB() const -> AABB {
        return _bounding;
    }

    virtual auto getCentroid() const -> Vec3 {
        return _centroid;
    }

    auto getType() const -> PrimitiveType {
        return _type;
    }

protected:
    AABB          _bounding{};
    Vec3          _centroid{};
    PrimitiveType _type;
};
