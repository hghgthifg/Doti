export module Graphics.Shader;

import OpenGL;

import std;
import Core.Logger;
import Core.Math;

export class Shader {
public:
    Shader() : _ID(0), _name("null") {};

    Shader(Shader&& other) noexcept {
        std::swap(_ID, other._ID);
        std::swap(_name, other._name);
        other.~Shader();
    };

    ~Shader() {
        if (_ID != 0) {
            Logger::debug("Destroying shader: " + _name + ", ID: " + std::to_string(_ID));
            glDeleteProgram(_ID);
        }
    }

    auto operator =(const Shader&) -> Shader& = delete;

    auto operator=(Shader&& other) noexcept -> Shader& {
        if (this != &other) {
            std::swap(_ID, other._ID);
            std::swap(_name, other._name);
            other.~Shader();
        }
        return *this;
    }

    auto activate() const -> void {
        glUseProgram(_ID);
    }

    auto deactivate() const -> void {
        glUseProgram(0);
    }

    static Shader loadFromFile(const std::string& name, const std::string& vertex_path,
                               const std::string& fragment_path) {
        if (vertex_path == "" || fragment_path == "") {
            Logger::error("No shader paths provided.");
            return Shader();
        }
        std::string   vertexCode;
        std::string   fragmentCode;
        std::ifstream vertexFile;
        std::ifstream fragmentFile;

        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        /* Load vertex shader from file */
        try {
            vertexFile.open(vertex_path);
            std::stringstream vertexStream;
            vertexStream << vertexFile.rdbuf();
            vertexFile.close();
            vertexCode = vertexStream.str();
        } catch (...) {
            Logger::error("Failed to open vertex shader file: " + vertex_path);
            return Shader();
        }

        /* Load fragment shader from file */
        try {
            fragmentFile.open(fragment_path);
            std::stringstream fragmentStream;
            fragmentStream << fragmentFile.rdbuf();
            fragmentFile.close();
            fragmentCode = fragmentStream.str();
        } catch (...) {
            Logger::error("Failed to open fragment shader file: " + fragment_path);
            return Shader();
        }

        return loadFromString(name, vertexCode, fragmentCode);
    }

    static Shader loadFromString(const std::string& name, const std::string& vertex_code,
                                 const std::string& fragment_code) {
        /* Compile Shader Source */
        Shader      result;
        const char* vertexCodeCStr   = vertex_code.c_str();
        const char* fragmentCodeCStr = fragment_code.c_str();

        const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexCodeCStr, nullptr);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, "VERTEX");

        const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentCodeCStr, nullptr);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, "FRAGMENT");

        /* Link shaders */
        result._ID = glCreateProgram();
        Logger::info("Created shader: " + result._name + ", ID: " + std::to_string(result._ID));
        glAttachShader(result._ID, vertexShader);
        glAttachShader(result._ID, fragmentShader);
        glLinkProgram(result._ID);
        checkCompileErrors(result._ID, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        result._name = name;
        return result;
    }

    static Shader loadFromBin(const std::string&   name,
                              const unsigned char* vertex_shader,
                              const unsigned int&  vertex_shader_size,
                              const unsigned char* fragment_shader,
                              const unsigned int&  fragment_shader_size) {
        Shader result;

        const auto vert = glCreateShader(GL_VERTEX_SHADER);
        const auto frag = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderBinary(1, &vert, GL_SHADER_BINARY_FORMAT_SPIR_V, vertex_shader,
                       vertex_shader_size * sizeof(unsigned char));
        checkCompileErrors(vert, "VERTEX");

        glShaderBinary(1, &frag, GL_SHADER_BINARY_FORMAT_SPIR_V, fragment_shader,
                       fragment_shader_size * sizeof(unsigned char));
        checkCompileErrors(frag, "FRAGMENT");

        result._ID = glCreateProgram();
        Logger::info("Created shader: " + result._name + ", ID: " + std::to_string(result._ID));
        glAttachShader(result._ID, vert);
        glAttachShader(result._ID, frag);
        glLinkProgram(result._ID);
        checkCompileErrors(result._ID, "PROGRAM");

        glDeleteShader(vert);
        glDeleteShader(frag);
        result._name = name;
        return result;
    }

    // ------------------------------------------------------------------------
    auto setBool(const std::string& name, const bool value) const -> void {
        glUniform1i(glGetUniformLocation(_ID, name.c_str()), (int) value);
    }

    // ------------------------------------------------------------------------
    auto setInt(const std::string& name, const int value) const -> void {
        glUniform1i(glGetUniformLocation(_ID, name.c_str()), value);
    }

    // ------------------------------------------------------------------------
    auto setFloat(const std::string& name, const float value) const -> void {
        glUniform1f(glGetUniformLocation(_ID, name.c_str()), value);
    }

    // ------------------------------------------------------------------------
    auto setVec2(const std::string& name, const Vec2& value) const -> void {
        glUniform2fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]);
    }

    auto setVec2(const std::string& name, const float x, const float y) const -> void {
        glUniform2f(glGetUniformLocation(_ID, name.c_str()), x, y);
    }

    // ------------------------------------------------------------------------
    auto setVec3(const std::string& name, const Vec3& value) const -> void {
        glUniform3fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]);
    }

    auto setVec3(const std::string& name, const float x, const float y, const float z) const -> void {
        glUniform3f(glGetUniformLocation(_ID, name.c_str()), x, y, z);
    }

    // ------------------------------------------------------------------------
    auto setVec4(const std::string& name, const Vec4& value) const -> void {
        glUniform4fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]);
    }

    auto setVec4(const std::string& name, const float x, const float y, const float z, const float w) const -> void {
        glUniform4f(glGetUniformLocation(_ID, name.c_str()), x, y, z, w);
    }

    // ------------------------------------------------------------------------
    auto setMat2(const std::string& name, const Mat2& mat) const -> void {
        glUniformMatrix2fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // ------------------------------------------------------------------------
    auto setMat3(const std::string& name, const Mat3& mat) const -> void {
        glUniformMatrix3fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // ------------------------------------------------------------------------
    auto setMat4(const std::string& name, const Mat4& mat) const -> void {
        glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    static auto checkCompileErrors(const uint32_t shader, const std::string_view type) -> void {
        int  success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                Logger::error(infoLog);
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                Logger::error(infoLog);
            }
        }
    }

    uint32_t    _ID;
    std::string _name;
};
