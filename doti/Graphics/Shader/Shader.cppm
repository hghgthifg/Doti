export module Graphics.Shader;

import OpenGL;

import std;
import Debug.Logger;
import Math;

export class Shader {
public:
    Shader() = delete;

    Shader(const Shader&) = delete;

    explicit Shader(const std::string& name) : _ID(0), _name(name) {
        Logger::info("Created shader: " + name);
    }

    Shader(Shader&&) = delete;

    ~Shader() {
        glDeleteProgram(_ID);
    }

    auto operator =(const Shader&) -> Shader& = delete;

    auto operator =(Shader&&) -> Shader& = default;

    Shader(const std::string& name, const std::string& vertexPath,
           const std::string& fragmentPath): _ID(0), _name(name) {
        Logger::info("Created shader: " + name);
        Logger::info("Loaded vertex shader: " + vertexPath + " and fragment shader: " + fragmentPath);
        this->_vertexPath   = vertexPath;
        this->_fragmentPath = fragmentPath;
        this->loadShader();
    }

    auto activate() const -> void {
        glUseProgram(_ID);
    }

    auto deactivate() const -> void {
        glUseProgram(0);
    }

    // ------------------------------------------------------------------------
    auto loadShader() -> void {
        // 1. Load shader code from file
        if (_vertexPath == "" || _fragmentPath == "") {
            Logger::error("No shader paths provided.");
            return;
        }
        std::string   vertexCode;
        std::string   fragmentCode;
        std::ifstream vertexFile;
        std::ifstream fragmentFile;

        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        /* Load vertex shader from file */
        try {
            vertexFile.open(_vertexPath);
            std::stringstream vertexStream;
            vertexStream << vertexFile.rdbuf();
            vertexFile.close();
            vertexCode = vertexStream.str();
        } catch (const std::ifstream::failure& e) {
            Logger::error("Failed to open vertex shader file: " + _vertexPath);
            return;
        }

        /* Load fragment shader from file */
        try {
            fragmentFile.open(_fragmentPath);
            std::stringstream fragmentStream;
            fragmentStream << fragmentFile.rdbuf();
            fragmentFile.close();
            fragmentCode = fragmentStream.str();
        } catch (const std::ifstream::failure& e) {
            Logger::error("Failed to open fragment shader file: " + _fragmentPath);
            return;
        }

        // 2. Compile shaders
        const char* vertexCodeCStr   = vertexCode.c_str();
        const char* fragmentCodeCStr = fragmentCode.c_str();
        uint32_t    vertexShader, fragmentShader;

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexCodeCStr, nullptr);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, "VERTEX");

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentCodeCStr, nullptr);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, "FRAGMENT");

        // 3. Link shaders
        _ID = glCreateProgram();
        glAttachShader(_ID, vertexShader);
        glAttachShader(_ID, fragmentShader);
        glLinkProgram(_ID);
        checkCompileErrors(_ID, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        _shaders.insert({_name, this});

        Logger::info("Successfully compiled and linked shader: " + _vertexPath + " and " + _fragmentPath);
    }

    auto loadShader(const std::string& vertexPath, const std::string& fragmentPath) -> void {
        Logger::info("Loaded vertex shader: " + vertexPath + "and fragment shader: " + fragmentPath);
        this->_vertexPath   = vertexPath;
        this->_fragmentPath = fragmentPath;
        loadShader();
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
        glUniform2fv(glGetUniformLocation(_ID, name.c_str()), 1, &value(0));
    }

    auto setVec2(const std::string& name, const float x, const float y) const -> void {
        glUniform2f(glGetUniformLocation(_ID, name.c_str()), x, y);
    }

    // ------------------------------------------------------------------------
    auto setVec3(const std::string& name, const Vec3& value) const -> void {
        glUniform3fv(glGetUniformLocation(_ID, name.c_str()), 1, &value(0));
    }

    auto setVec3(const std::string& name, const float x, const float y, const float z) const -> void {
        glUniform3f(glGetUniformLocation(_ID, name.c_str()), x, y, z);
    }

    // ------------------------------------------------------------------------
    auto setVec4(const std::string& name, const Vec4& value) const -> void {
        glUniform4fv(glGetUniformLocation(_ID, name.c_str()), 1, &value(0));
    }

    auto setVec4(const std::string& name, const float x, const float y, const float z, const float w) const -> void {
        glUniform4f(glGetUniformLocation(_ID, name.c_str()), x, y, z, w);
    }

    // ------------------------------------------------------------------------
    auto setMat2(const std::string& name, const Mat2& mat) const -> void {
        glUniformMatrix2fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat(0, 0));
    }

    // ------------------------------------------------------------------------
    auto setMat3(const std::string& name, const Mat3& mat) const -> void {
        glUniformMatrix3fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat(0, 0));
    }

    // ------------------------------------------------------------------------
    auto setMat4(const std::string& name, const Mat4& mat) const -> void {
        glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat(0, 0));
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

    uint32_t                               _ID;
    std::string                            _name;
    std::string                            _vertexPath;
    std::string                            _fragmentPath;
    static std::map<std::string, Shader *> _shaders;
};

std::map<std::string, Shader *> Shader::_shaders;
