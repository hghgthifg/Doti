export module Graphics.Accleration.BVHTree;

import std;
import Math;
import Graphics.Accleration.BoundingBox;
import Geometry.Triangle;

export class BVHTree {
public:
    explicit BVHTree(const int32_t maxPrimitivesInNode = 1) : _maxPrimitivesInNode(maxPrimitivesInNode) {}

    auto build(std::vector<std::shared_ptr<Triangle>> primitives) -> void {
        _primitives = std::move(primitives);
        if (_primitives.empty()) return;

        auto getTriangleBound = [](const std::shared_ptr<Triangle>& t) {
            return BoundingBox{Math::min(t->v1, t->v2, t->v3), Math::max(t->v1, t->v2, t->v3)};
        };

        /* 1. Initialize primitive information */
        std::vector<BVHPrimitiveInfo> primitiveInfo;
        primitiveInfo.reserve(_primitives.size());
        for (int32_t i = 0; i < _primitives.size(); i++) {
            primitiveInfo.emplace_back(i, getTriangleBound(_primitives[i]));
        }

        /* 2. Build the BVH tree */
        int32_t                                totalNodes = 0;
        std::vector<std::shared_ptr<Triangle>> orderedPrimitives;
        orderedPrimitives.reserve(_primitives.size());
        auto root = recursiveBuild(primitiveInfo, 0, static_cast<int32_t>(_primitives.size()), totalNodes,
                                   orderedPrimitives);
        std::swap(orderedPrimitives, _primitives);
        primitiveInfo.clear();

        /* 3. Flatten the BVH tree into a linear array */
        _nodeCount = totalNodes;
        _nodes.resize(_nodeCount);
        int32_t offset = 0;
        flattenBVHTree(std::move(root), offset);

        /* 4. Initialize mesh data */
        _meshCount          = static_cast<int32_t>(_primitives.size());
        int32_t meshNumSize = _meshCount * (9 + 9 + 6);
        float   mesh_x_f    = Math::sqrt(meshNumSize);
        _meshTextureX       = Math::ceil(mesh_x_f);
        _meshTextureY       = Math::ceil(static_cast<float>(meshNumSize) / static_cast<float>(_meshTextureX));
        _meshData.resize((_meshTextureX * _meshTextureY) * (9 + 9 + 6));

        /* 5. Assign vertex values */
        for (int32_t i = 0; i < _meshCount; i++) {
            const auto& triangle  = _primitives[i];
            _meshData[i * 24 + 0] = triangle->v1.x;
            _meshData[i * 24 + 1] = triangle->v1.y;
            _meshData[i * 24 + 2] = triangle->v1.z;
            _meshData[i * 24 + 3] = triangle->v2.x;
            _meshData[i * 24 + 4] = triangle->v2.y;
            _meshData[i * 24 + 5] = triangle->v2.z;
            _meshData[i * 24 + 6] = triangle->v3.x;
            _meshData[i * 24 + 7] = triangle->v3.y;
            _meshData[i * 24 + 8] = triangle->v3.z;
            // Add normals and texture coordinates if needed
        }
    }

private:
    struct BVHNode {
        BoundingBox              boundingBox{};
        std::unique_ptr<BVHNode> left           = nullptr;
        std::unique_ptr<BVHNode> right          = nullptr;
        int32_t                  splitAxis      = 0;
        int32_t                  firstOffset    = 0;
        int32_t                  primitiveCount = 0;
    };

    struct BVHPrimitiveInfo {
        int32_t     primitiveIndex;
        BoundingBox boundingBox;
        Vec3        centroid;

        BVHPrimitiveInfo(const int32_t index, const BoundingBox& bounds) : primitiveIndex(index),
                                                                           boundingBox(bounds),
                                                                           centroid(
                                                                               0.5f * (bounds.getMin() + bounds.
                                                                                   getMax())
                                                                           ) {}
    };

    struct LinearBVHNode {
        BoundingBox boundingBox;

        union {
            int32_t primitivesOffset;  // Offset of the primitives in a leaf node
            int32_t secondChildOffset; // Offset of the second child in an internal node
        };

        uint8_t primitivesCount = 0; // When it is greater than 0, it indicates a leaf node
        uint8_t axis            = 0; // 0 -> x axis, 1 -> y axis, 2 -> z axis
    };

    auto recursiveBuild(
        std::vector<BVHPrimitiveInfo>&          primitiveInfo,
        const int32_t                           start,
        const int32_t                           end,
        int32_t&                                totalNodes,
        std::vector<std::shared_ptr<Triangle>>& orderedPrimitives) -> std::unique_ptr<BVHNode> {
        auto node = std::make_unique<BVHNode>();
        totalNodes += 1;

        // Calculate the bounding box for the current node
        BoundingBox bounds;
        for (int32_t i = start; i < end; i++) {
            bounds.mergeWith(primitiveInfo[i].boundingBox);
        }

        const int32_t primitiveCount = end - start;

        if (primitiveCount <= _maxPrimitivesInNode) {
            // Create a leaf node
            node->firstOffset    = static_cast<int32_t>(orderedPrimitives.size());
            node->primitiveCount = primitiveCount;
            for (int32_t i = start; i < end; i++) {
                const int32_t index = static_cast<int32_t>(primitiveInfo[i].primitiveIndex);
                orderedPrimitives.emplace_back(_primitives[index]);
            }
            node->boundingBox = bounds;
        } else {
            // Calculate the bounding box of centroids
            BoundingBox centroidBounds;
            for (int32_t i = start; i < end; i++) {
                centroidBounds.mergeWith(primitiveInfo[i].boundingBox);
            }
            uint8_t dim = centroidBounds.getMaximumExtent();

            // If centroids are the same along the split axis, create a leaf node
            if (centroidBounds.getMax()[dim] == centroidBounds.getMin()[dim]) {
                node->firstOffset    = static_cast<int32_t>(orderedPrimitives.size());
                node->primitiveCount = primitiveCount;
                for (int32_t i = start; i < end; i++) {
                    const int32_t index = static_cast<int32_t>(primitiveInfo[i].primitiveIndex);
                    orderedPrimitives.emplace_back(_primitives[index]);
                }
                node->boundingBox = centroidBounds;
            } else {
                // Sort primitives based on the centroid along the split axis
                const int32_t mid = (start + end) / 2;
                std::nth_element(
                    primitiveInfo.begin() + start,
                    primitiveInfo.begin() + mid,
                    primitiveInfo.begin() + end,
                    [dim](const BVHPrimitiveInfo& a, const BVHPrimitiveInfo& b) {
                        return a.centroid[dim] < b.centroid[dim];
                    }
                );

                // Recursively build child nodes
                node->left        = recursiveBuild(primitiveInfo, start, mid, totalNodes, orderedPrimitives);
                node->right       = recursiveBuild(primitiveInfo, mid, end, totalNodes, orderedPrimitives);
                node->splitAxis   = dim;
                node->boundingBox = bounds;
            }
        }
        return node;
    }

    auto flattenBVHTree(const std::unique_ptr<BVHNode>& node, int32_t& offset) -> int32_t {
        LinearBVHNode& linearNode = _nodes[offset];
        linearNode.boundingBox    = node->boundingBox;
        const int32_t myOffset    = offset++;

        if (node->primitiveCount > 0) {
            // Leaf node
            linearNode.primitivesOffset = node->firstOffset;
            linearNode.primitivesCount  = static_cast<uint8_t>(node->primitiveCount);
        } else {
            // Internal node
            linearNode.axis              = static_cast<uint8_t>(node->splitAxis);
            linearNode.primitivesCount   = 0;
            linearNode.secondChildOffset = flattenBVHTree(std::move(node->right), offset);
            flattenBVHTree(std::move(node->left), offset);
        }
        return myOffset;
    }

    std::vector<LinearBVHNode>             _nodes;            /*!< Linear array of BVH nodes */
    std::vector<std::shared_ptr<Triangle>> _primitives;       /*!< Ordered list of primitives */
    std::vector<float>                     _meshData;         /*!< Mesh vertex data */
    int32_t                                _nodeCount    = 0; /*!< Total number of nodes in the BVH */
    int32_t                                _nodeTextureX = 0, _nodeTextureY = 0;
    int32_t                                _meshTextureX = 0, _meshTextureY = 0;
    int32_t                                _meshCount    = 0;    /*!< Number of meshes */
    int32_t                                _maxPrimitivesInNode; /*!< Maximum number of primitives allowed in a node */
};
