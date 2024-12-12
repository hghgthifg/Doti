export module Graphics.Render.Acceleration.BVHTree;

import std;
import Math;
import Debug.Logger;
import Graphics.Buffer;
import Graphics.Render.Acceleration.AABB;

enum class BVHSplitMethod { SAH, Middle, EqualCounts };

struct BVHNode {
    AABB    box;
    int32_t sizeIndex;
};

struct BVHBuffer {
    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;
    std::shared_ptr<Buffer> nodeBuffer;
};


struct Mesh {
    std::vector<Vec3>     vertices;
    std::vector<Vec3>     normals;
    std::vector<Vec3>     texCoords;
    std::vector<uint32_t> indices;
};

constexpr int32_t paraLim = 10000;

export class BVHTree {
public:
    BVHTree(const std::vector<Mesh>& meshes, BVHSplitMethod splitMethod) : _splitMethod(splitMethod) {
        uint32_t indicesCount = 0;
        for (const auto& mesh: meshes) {
            _vertices.insert(_vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
            _normals.insert(_normals.end(), mesh.normals.begin(), mesh.normals.end());

            std::vector<uint32_t> transIndices(mesh.indices.size());
            const uint32_t        meshIndex = &mesh - &meshes[0];

            std::ranges::transform(mesh.indices, transIndices.begin(),
                                   [meshIndex, indicesCount](const uint32_t x) {
                                       return (x + indicesCount) | (meshIndex << 24);
                                   });
            // 8bit meshIndex | 24bit vertexIndex

            _indices.insert(_indices.end(), transIndices.begin(), transIndices.end());
            indicesCount += mesh.indices.size();
        }

        std::vector<HittableInfo> hittableInfo(indicesCount / 3);

        const auto clamp = [](uint32_t x) { return x & 0x00ffffff; };

        AABB bound;
        for (uint32_t i = 0; i < hittableInfo.size(); ++i) {
            HittableInfo hInfo;
            hInfo.bound = AABB(
                _vertices[clamp(_indices[i * 3 + 0])],
                _vertices[clamp(_indices[i * 3 + 1])],
                _vertices[clamp(_indices[i * 3 + 2])]
            );
            hInfo.centroid = hInfo.bound.getCentroid();
            hInfo.index    = i;
            bound.mergeWith(hInfo.bound);
            hittableInfo[i] = hInfo;
        }

        _nodes.reserve(hittableInfo.size() * 2 - 1);
        build(0, hittableInfo, bound, 0, hittableInfo.size() - 1);
    }

    ~BVHTree();

    auto genBuffers() -> BVHBuffer {
        const auto vertexBuf = std::make_shared<Buffer>();
        const auto indexBuf  = std::make_shared<Buffer>();
        const auto nodeBuf   = std::make_shared<Buffer>();

        vertexBuf->allocate(_vertices.size() * sizeof(Vec3), _vertices.data(), 0);
        indexBuf->allocate(_indices.size() * sizeof(uint32_t), _indices.data(), 0);
        nodeBuf->allocate(_nodes.size() * sizeof(BVHNode), _nodes.data(), 0);

        return {vertexBuf, indexBuf, nodeBuf};
    }

private:
    struct HittableInfo {
        AABB    bound;
        Vec3    centroid;
        int32_t index;
    };

    void build(int32_t       offset, std::vector<HittableInfo>& hittableInfo, const AABB& nodeBound, const int32_t l,
               const int32_t r) {
        int8_t  dim    = nodeBound.maxExtent();
        int32_t size   = (r - l) * 2 + 1;
        _nodes[offset] = BVHNode{nodeBound, (size == 1) ? -l : size};

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
            std::sort(hittableInfo.begin() + l, hittableInfo.end() + r, cmp);
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
                    const float c = boundInfo[i].getSurfaceArea() * (i + 1) +
                                    boundInfoRev[i + 1].getSurfaceArea() * (hittableCount - i - 1);
                    if (c < cost) {
                        cost = c, m = l + i;
                    }
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

    BVHSplitMethod        _splitMethod;
    std::vector<Vec3>     _vertices;
    std::vector<Vec3>     _normals;
    std::vector<uint32_t> _indices;
    std::vector<BVHNode>  _nodes;
};
