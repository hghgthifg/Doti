export module Graphics.Material;

import std;
import Math;
import OpenGL;

// Unified value representation for material properties
export class MaterialValue {
public:
    MaterialValue() = default;

    MaterialValue(float scalar) : _type(Type::Scalar), _scalar(scalar) {}
    MaterialValue(const Vec3& vec) : _type(Type::Vec3), _vec3(vec) {}
    MaterialValue(const Vec4& vec) : _type(Type::Vec4), _vec4(vec) {}

    MaterialValue(GLuint texId, std::string path)
        : _type(Type::Texture), _textureId(texId), _texturePath(std::move(path)) {}

    auto isTexture() const noexcept -> bool { return _type == Type::Texture; }

    auto getTexture() const -> std::pair<GLuint, const std::string &> {
        return {_textureId, _texturePath};
    }

    template<typename T>
    auto getValue() const -> std::optional<T> {
        if constexpr (std::is_same_v<T, float>) {
            return _type == Type::Scalar ? std::make_optional(_scalar) : std::nullopt;
        } else if constexpr (std::is_same_v<T, Vec3>) {
            return _type == Type::Vec3 ? std::make_optional(_vec3) : std::nullopt;
        } else if constexpr (std::is_same_v<T, Vec4>) {
            return _type == Type::Vec4 ? std::make_optional(_vec4) : std::nullopt;
        }
        return std::nullopt;
    }

private:
    enum class Type {
        Scalar,
        Vec3,
        Vec4,
        Texture
    } _type{Type::Scalar};

    float       _scalar{0.0f};
    Vec3        _vec3{};
    Vec4        _vec4{};
    GLuint      _textureId{0};
    std::string _texturePath{};
};

export enum class MaterialPropertyType {
    BaseColor, // Diffuse/Albedo
    Normal,    // Normal mapping
    Metallic,  // PBR metallic
    Roughness, // PBR roughness/Traditional smoothness
    AmbientOcclusion,
    Emissive,
    Height,  // Displacement/Parallax
    Opacity, // Transparency
    IOR,     // Index of refraction
    // Add more as needed...
};

export class Material {
public:
    Material() = default;

    explicit Material(std::string name) : _name(std::move(name)) {}

    // Property setters
    auto setProperty(MaterialPropertyType type, float value) -> void {
        _properties[type] = MaterialValue(value);
    }

    auto setProperty(MaterialPropertyType type, const Vec3& value) -> void {
        _properties[type] = MaterialValue(value);
    }

    auto setProperty(MaterialPropertyType type, const Vec4& value) -> void {
        _properties[type] = MaterialValue(value);
    }

    auto setProperty(MaterialPropertyType type, GLuint texId, std::string path) -> void {
        _properties[type] = MaterialValue(texId, std::move(path));
    }

    // Property getters with type safety
    template<typename T>
    auto getProperty(MaterialPropertyType type) const -> std::optional<T> {
        if (auto it = _properties.find(type); it != _properties.end()) {
            return it->second.getValue<T>();
        }
        return std::nullopt;
    }

    auto getTextureProperty(MaterialPropertyType type) const
        -> std::optional<std::pair<GLuint, const std::string &>> {
        if (auto it = _properties.find(type);
            it != _properties.end() && it->second.isTexture()) {
            return it->second.getTexture();
        }
        return std::nullopt;
    }

    // Create common material presets
    static auto createDefaultMaterial() -> Material {
        Material mat("default_material");
        mat.setProperty(MaterialPropertyType::BaseColor, Vec3(0.8f));
        mat.setProperty(MaterialPropertyType::Metallic, 0.0f);
        mat.setProperty(MaterialPropertyType::Roughness, 0.5f);
        mat.setProperty(MaterialPropertyType::AmbientOcclusion, 1.0f);
        return mat;
    }

    // Clone support
    auto clone() const -> Material {
        Material newMat(_name);
        newMat._properties = _properties;
        return newMat;
    }

    // Utility
    auto getName() const -> const std::string& { return _name; }

private:
    std::string                                             _name;
    std::unordered_map<MaterialPropertyType, MaterialValue> _properties;
};
