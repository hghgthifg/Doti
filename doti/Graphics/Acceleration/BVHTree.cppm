export module Graphics.Render.Acceleration.BVHTree;

import std;
import Core.Math;
import Core.Logger;
import Graphics.Buffer;
import Graphics.Render.Acceleration.AABB;
import Graphics.Render.Acceleration.Primitive;

enum class BVHSplitMethod { SAH, Middle, EqualCounts };

struct BVHNode {
    AABB    box;
    int32_t index; // max size of the tree if > 0, primitive index if < 0
    float   padding[3] = {0.0f, 0.0f, 0.0f};
};

constexpr uint32_t paraLim = 10000;

export class BVHTree {
public:
    BVHTree() = default;

    ~BVHTree() = default;

    BVHTree(const std::vector<std::shared_ptr<PrimitiveBase>>& primitives,
            BVHSplitMethod split_method = BVHSplitMethod::EqualCounts): _splitMethod(split_method) {
        std::vector<HittableInfo> hittableInfo(primitives.size());
        AABB                      bound;
        uint32_t                  triangle_count = 0;
        uint32_t                  sphere_count   = 0;
        for (uint32_t i = 0; i < hittableInfo.size(); i++) {
            HittableInfo hInfo;
            const auto   pri = primitives[i];
            hInfo.bound      = pri->getAABB();
            hInfo.centroid   = pri->getCentroid();

            uint32_t masked_index;
            switch (pri->getType()) {
                case PrimitiveType::Triangle: {
                    masked_index = triangle_count | 0x10000000;
                    triangle_count++;
                    break;
                }
                case PrimitiveType::Sphere: {
                    masked_index = sphere_count | 0x20000000;
                    sphere_count++;
                    break;
                }
                default: {
                    Logger::error("Unknown PrimitiveType!");
                    continue;
                }
            }
            /* 1bit reserved + 3bit primitive type + 28bit index */
            /*
             * Primitive type:
             * 0001 - Triangle
             * 0002 - Sphere
             */
            hInfo.index = masked_index;
            bound.mergeWith(hInfo.bound);
            hittableInfo[i] = hInfo;
        }
        _nodes.resize(hittableInfo.size() * 2 - 1);
        this->build(0, hittableInfo, bound, 0, hittableInfo.size() - 1);
    }

    auto genNodeBuffer() {
        const auto nodeBuf = std::make_shared<Buffer>();

        nodeBuf->allocate(_nodes.size() * sizeof(BVHNode), _nodes.data(), 0);
        return nodeBuf;
    }

private:
    struct HittableInfo {
        AABB     bound;
        Vec3     centroid;
        uint32_t index;
    };

    void build(const int32_t offset, std::vector<HittableInfo>& hittableInfo, const AABB& nodeBound, const int32_t l,
               const int32_t r) {
        int8_t  dim    = nodeBound.maxExtent();
        int32_t size   = (r - l + 1) * 2 - 1;
        _nodes[offset] = BVHNode{nodeBound, (size == 1) ? -static_cast<int32_t>(hittableInfo[l].index) : size};

        if (l == r) return;

        const auto getDim = [](const Vec3& v, int8_t d) {
            if (d == 0) return v.x;
            if (d == 1) return v.y;
            if (d == 2) return v.z;
            Logger::error("Unknown dimension");
            return 0.f;
        };

        const auto cmp = [dim, getDim](const HittableInfo& a, HittableInfo& b) {
            return getDim(a.centroid, dim) < getDim(b.centroid, dim);
        };

        if (hittableInfo.size() > paraLim) {
            std::sort(std::execution::par, hittableInfo.begin() + l, hittableInfo.begin() + r, cmp);
        } else {
            std::sort(hittableInfo.begin() + l, hittableInfo.begin() + r, cmp);
        }
        int32_t hittableCount = r - l + 1;

        if (hittableCount == 2) {
            build(offset + 1, hittableInfo, hittableInfo[l].bound, l, l);
            build(offset + 2, hittableInfo, hittableInfo[r].bound, r, r);
            return;
        }

        AABB* boundInfo    = new AABB[hittableCount];
        AABB* boundInfoRev = new AABB[hittableCount];

        boundInfo[0]                    = hittableInfo[l].bound;
        boundInfoRev[hittableCount - 1] = hittableInfo[r].bound;

        for (int32_t i = 1; i < hittableCount; ++i) {
            boundInfo[i]                        = AABB(boundInfo[i - 1], hittableInfo[l + i].bound);
            boundInfoRev[hittableCount - 1 - i] = AABB(boundInfoRev[hittableCount - i], hittableInfo[r - i].bound);
        }

        int32_t m = l;
        switch (_splitMethod) {
            case BVHSplitMethod::SAH: {
                float cost = boundInfo[0].getSurfaceArea() + boundInfoRev[1].getSurfaceArea() * (r - l);
                for (int32_t i = 1; i < hittableCount - 1; i++) {
                    const float c = boundInfo[i].getSurfaceArea() * (i + 1)
                                    + boundInfoRev[i + 1].getSurfaceArea() * (hittableCount - i - 1);
                    if (c < cost) { cost = c, m = l + i; }
                }
                break;
            }
            case BVHSplitMethod::Middle: {
                const Vec3  nodeCentroid = nodeBound.getCentroid();
                const float mid          = getDim(nodeCentroid, dim);
                for (m = l; m < r - l; m++) {
                    const float tmp = getDim(hittableInfo[m].centroid, dim);
                    if (tmp >= mid) break;
                }
                break;
            }
            case BVHSplitMethod::EqualCounts: {
                m = (l + r) >> 1;
                break;
            }
            default: break;
        }

        const AABB lBound = boundInfo[m - l];
        const AABB rBound = boundInfoRev[m + 1 - l];
        delete[] boundInfo;
        delete[] boundInfoRev;

        #pragma omp parallel sections
        {
            #pragma omp section
            build(offset + 1, hittableInfo, lBound, l, m);

            #pragma omp section
            build(offset + 2 * (m - l) + 2, hittableInfo, rBound, m + 1, r);
        }
    }

    BVHSplitMethod       _splitMethod;
    std::vector<BVHNode> _nodes;
};
