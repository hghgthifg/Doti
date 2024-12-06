export module Graphics.Render.BVHTree;

import std;
import Math;
import Graphics.Render.BoundingBox;

struct BVHNode {
    BVHNode*    lchild;
    BVHNode*    rchild;
    int32_t     splitAxis, firstPrimOffset, primitiveCount;
    BoundingBox box;

    auto initLeaf(int32_t first, int n, const BoundingBox& b) -> void {
        firstPrimOffset = first;
        primitiveCount  = n;
        box             = b;
        lchild          = nullptr;
        rchild          = nullptr;
    }

    auto initInterior(int32_t axis, BVHNode* l, BVHNode* r) -> void {
        lchild         = l;
        rchild         = r;
        splitAxis      = axis;
        box            = BoundingBox::merge(l->box, r->box);
        primitiveCount = 0;
    }
};

struct LinearBVHNode {
    Vec3  pMin, pMax;
    float primitiveCount;
    float axis;
    float childOffset;

    void setBoundingBox(const BoundingBox& b) {
        pMin = b.getMin();
        pMax = b.getMax();
    }

    void getBoundingBox(BoundingBox& b) const {
        b.setMin(pMin);
        b.setMax(pMax);
    }
};

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo(const int32_t primitiveCount, const BoundingBox& b) : primitiveCount(primitiveCount), bound(b),
                                                                           centroid(
                                                                               0.5f * b.getMin() + 0.5f * b.getMax()) {}

    int32_t     primitiveCount;
    BoundingBox bound;
    Vec3        centroid;
};

struct BucketInfo {
    int32_t     count = 0;
    BoundingBox bounds;
};

export class BVHTree {
public:
};
