export module Graphics.Render.Drawable.Model;

import assimp;
import std;
import Math;
import OpenGL;
import Debug.Logger;
import Graphics.Material;
import Graphics.Render.RenderContext;
import Graphics.Render.Drawable.Mesh;
import Graphics.Loader.Image.Jxl;

// Represents a node in the model hierarchy
export struct ModelNode {
    std::string                             name;
    Mat4                                    transform{1.0f};
    std::vector<size_t>                     meshIndices;
    std::vector<std::unique_ptr<ModelNode>> children;
};

export struct ModelData {
    std::vector<Mesh>          meshes;
    std::vector<Material>      materials;
    std::unique_ptr<ModelNode> rootNode;
};

export class Model {
public:
    Model() = default;

    /*!
     * @brief Construct a model from loaded model data
     * @param data ModelData containing meshes, materials and node hierarchy
     * @note Takes ownership of the model data using move semantics
     */
    Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials, std::unique_ptr<ModelNode> root_node)
        : _meshes(std::move(meshes))
          , _materials(std::move(materials))
          , _rootNode(std::move(root_node)) {}

    Model(ModelData&& model_data)
        : _meshes(std::move(model_data.meshes)),
          _materials(std::move(model_data.materials)),
          _rootNode(std::move(model_data.rootNode)) {}

    auto draw(RenderContext& context) const -> void {
        if (_rootNode) {
            drawNode(*_rootNode, context);
        }
    }

    /*!
     * @brief Get material by index
     * @param index Index of the material to retrieve
     * @return Optional reference to the material if index is valid
     */
    [[nodiscard]] auto getMaterial(size_t index) const
        -> std::optional<std::reference_wrapper<const Material>> {
        if (index < _materials.size()) {
            return std::ref(_materials[index]);
        }
        return std::nullopt;
    }

    /*!
     * @brief Get total number of meshes in the model
     */
    [[nodiscard]] auto getMeshCount() const noexcept -> size_t {
        return _meshes.size();
    }

    /*!
     * @brief Get total number of materials in the model
     */
    [[nodiscard]] auto getMaterialCount() const noexcept -> size_t {
        return _materials.size();
    }

private:
    /*!
     * @brief Recursively draw a node and its children
     * @param node Current node to draw
     * @param context Current render context
     */
    auto drawNode(const ModelNode& node, RenderContext& context) const -> void {
        // Save current transform
        const auto previousTransform = context.getTransform();

        // Apply node transform
        context.setTransform(previousTransform * node.transform);

        // Draw meshes
        for (const auto meshIndex: node.meshIndices) {
            if (meshIndex < _meshes.size()) {
                _meshes[meshIndex].draw(context);
            }
        }

        // Draw children
        for (const auto& child: node.children) {
            drawNode(*child, context);
        }

        // Restore transform
        context.setTransform(previousTransform);
    }

    std::vector<Mesh>          _meshes;
    std::vector<Material>      _materials;
    std::unique_ptr<ModelNode> _rootNode;
};
