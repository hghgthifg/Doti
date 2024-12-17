export module Scene.SceneNode;

import std;
import Core.Math;
import Graphics.Render.ObjectBase;
import Graphics.Render.RenderContext;

export class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    explicit SceneNode(const std::string& name, const std::string& type) : _name(name), _type(type) {}

    virtual ~SceneNode() = default;

    auto getParent() const -> std::shared_ptr<SceneNode> { return _parent.lock(); }
    auto setParent(const std::shared_ptr<SceneNode>& parent) -> void { _parent = parent; }
    auto setPosition(const Vec3& p) -> void { _position = p; }
    auto setScale(const Vec3& s) -> void { _scale = s; }
    auto setRotation(const Vec3& r) -> void { _rotation = r; }

    virtual bool isLeaf() const = 0;

    virtual void collectData() = 0;

protected:
    std::string              _name;
    std::string              _type;
    std::weak_ptr<SceneNode> _parent;

    Vec3 _position{0, 0, 0};
    Vec3 _scale{0, 0, 0};
    Vec3 _rotation{0, 0, 0};
};

export class LeafNode final : public SceneNode {
public:
    explicit LeafNode(const std::string& name, const std::string& type = "LeafNode") : SceneNode(name, type) {}
    auto     isLeaf() const -> bool override { return true; }

    auto collectData() -> void override {
        _drawable->sendDataToPipeline();
    }

    auto setDrawable(const std::shared_ptr<ObjectBase>& drawable) -> void { _drawable = drawable; }

private:
    std::shared_ptr<ObjectBase> _drawable;
};

export class CompositeNode final : public SceneNode {
public:
    explicit CompositeNode(const std::string& name, const std::string& type = "SceneNode") : SceneNode(name, type) {}
    auto     isLeaf() const -> bool override { return false; }

    auto collectData() -> void override {
        for (const auto& child: _children) {
            child->collectData();
        }
    }

    auto addChild(const std::shared_ptr<SceneNode>& child) -> void {
        if (child == nullptr) { return; }
        child->setParent(shared_from_this());
        _children.push_back(child);
    }

    auto removeChild(const std::shared_ptr<SceneNode>& child) -> void {
        if (child == nullptr) { return; }
        const auto it = std::ranges::find(_children, child);
        if (it == _children.end()) { return; }
        _children.erase(it);
    }

private:
    std::vector<std::shared_ptr<SceneNode>> _children;
};
