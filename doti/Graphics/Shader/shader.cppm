export module Graphics.Shader;

import OpenGL;

import std;
import Debug.Logger;
import Math;

export class ShaderFactory {};

export class Shader {
public:
    Shader() = delete;

    Shader(const Shader&) = delete;

    explicit Shader(const std::string& name) : _ID(0), _name(name) {
        Logger::info("Created shader: " + name);
    }

    Shader(Shader&&) = delete;

    Shader(const std::string& name, const std::string& vertexPath,
           const std::string& fragmentPath): _ID(0), _name(name) {
        Logger::info("Created shader: " + name);
        Logger::info("Loaded vertex shader: " + vertexPath + "and fragment shader: " + fragmentPath);
        this->_vertexPath   = vertexPath;
        this->_fragmentPath = fragmentPath;
    }

    auto LoadShader() -> void {
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

    auto LoadShader(const std::string& vertexPath, const std::string& fragmentPath) -> void {
        Logger::info("Loaded vertex shader: " + vertexPath + "and fragment shader: " + fragmentPath);
        this->_vertexPath   = vertexPath;
        this->_fragmentPath = fragmentPath;
        LoadShader();
    }

    auto setBool(const std::string& name, bool value) -> const void {
        glUniform1i(glGetUniformLocation(_ID, name.c_str()), (int) value);
    }

    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(_ID, name.c_str()), value);
    }

    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(_ID, name.c_str()), value);
    }

    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const Vec2 value) const {
        glUniform2fv(glGetUniformLocation(_ID, name.c_str()), 1, &value(0));
    }

    void setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(_ID, name.c_str()), x, y);
    }

    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const Vec3 value) const {
        glUniform3fv(glGetUniformLocation(_ID, name.c_str()), 1, &value(0));
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(_ID, name.c_str()), x, y, z);
    }

    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const Vec4 value) const {
        glUniform4fv(glGetUniformLocation(_ID, name.c_str()), 1, &value(0));
    }

    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(_ID, name.c_str()), x, y, z, w);
    }

    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const Mat2 mat) const {
        glUniformMatrix2fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat(0, 0));
    }

    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const Mat3 mat) const {
        glUniformMatrix3fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat(0, 0));
    }

    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const Mat4 mat) const {
        glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat(0, 0));
    }

private:
    static auto checkCompileErrors(uint32_t shader, std::string_view type) -> void {
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
