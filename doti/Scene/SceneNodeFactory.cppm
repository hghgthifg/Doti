export module Scene.SceneNodeFactory;

import std;
import Json;
import Core.Math;
import Core.Logger;
import Scene.SceneNode;
import Graphics.Render.Objects;

auto buildMesh(std::shared_ptr<LeafNode> node, const Json& data) -> std::shared_ptr<LeafNode> {
    if (!data.contains("mesh")) {
        Logger::error("Invalid mesh data : mesh not found!");
        return nullptr;
    }
    std::vector<Vec3>     normals;
    std::vector<Vec3>     vertices;
    std::vector<uint32_t> indices;
    // std::vector<uint32_t> textures;

    auto mesh = data.at("mesh");

    /* 1. Process vertices */
    if (mesh.contains("vertices") && mesh["vertices"].is_array()) {
        vertices.reserve(mesh.at("vertices").size());
        for (auto& v: mesh.at("vertices")) {
            vertices.emplace_back(
                Vec3{v[0].get<float>(), v[1].get<float>(), v[2].get<float>()}
            );
        }
        /* Process normals (optional) */
        if (mesh.contains("normals") && mesh["normals"].is_array()) {
            normals.resize(mesh.at("normals").size());
            if (mesh["normals"].size() != vertices.size()) {
                Logger::warning("Normals array size mismatch");
            }
            auto json_normals = mesh["normals"];
            for (uint64_t i = 0; i < json_normals.size(); i++) {
                auto n     = json_normals.at(i);
                normals[i] = Vec3{n[0].get<float>(), n[1].get<float>(), n[2].get<float>()};
            }
        }
        // /* Process texCoords (optional) */
        // if (mesh.contains("texCoords") && mesh["texCoords"].is_array()) {
        //     if (mesh["texCoords"].size() != vertices.size()) {
        //         Logger::warning("texCoords array size mismatch");
        //     }
        //     auto texCoords = mesh["texCoords"];
        //     for (uint64_t i = 0; i < texCoords.size(); i++) {
        //         auto t                = texCoords.at(i);
        //         vertices[i].texCoords = Vec2{t[0].get<float>(), t[1].get<float>()};
        //     }
        // }
    } else {
        Logger::error("Invalid mesh data : _vertices not found!");
        return nullptr;
    }

    /* Process indices */
    if (mesh.contains("indices") && mesh["indices"].is_array()) {
        indices.reserve(mesh.at("indices").size());
        if (mesh["indices"].size() % 3 != 0) {
            Logger::error("Indices array size mismatch! Index count: " + std::to_string(mesh["indices"].size()));
            return nullptr;
        }
        for (const auto& i: mesh["indices"]) {
            indices.emplace_back(i.get<uint32_t>());
        }
    }

    /* Get Material */
    uint32_t materialIndex = 0;
    if (data.contains("material_id")) {
        materialIndex = data.at("material_id").get<uint32_t>();
    }

    const auto object = std::make_shared<Mesh>(
        std::move(vertices), std::move(normals), std::move(indices), std::move(materialIndex)
    );
    node->hangObject(object);
    return node;
}

export class SceneNodeFactory {
public:
    static auto createNode(const Json& data) -> std::shared_ptr<SceneNode> {
        if (!data.contains("name")) {
            Logger::error("Invalid scene data : node name not found");
            return nullptr;
        }
        std::string name = data["name"].get<std::string>();
        std::string type = "container";
        if (data.contains("type")) {
            type = data["type"].get<std::string>();
        }
        if (type == "container") {
            return std::make_shared<CompositeNode>(name);
        } else if (type == "mesh") {
            return buildMesh(std::make_shared<LeafNode>(name, type), data);
        } else {
            return nullptr;
        }
    };
};
