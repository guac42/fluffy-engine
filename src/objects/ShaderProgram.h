#ifndef GAMEFRAME_SHADERPROGRAM_H
#define GAMEFRAME_SHADERPROGRAM_H

#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#define UPLOAD_VECTOR(n)                                                        \
void Upload(int location, const glm::vec##n& vec##n) const {                    \
    glProgramUniform##n##fv(this->id, location, 1, &vec##n[0]);                 \
}                                                                               \
                                                                                \
void Upload(const std::string& name, const glm::vec##n& vec##n) const {         \
    glProgramUniform##n##fv(this->id, GetUniformLocation(name), 1, &vec##n[0]); \
}

class Shader {
private:
    GLuint id = 0;

    static std::string getFileContents(const std::string& filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in)
            throw std::runtime_error(std::string("Cannot open shader file: ")+filename);

        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    }

public:
    const enum ShaderType {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER
    } shaderType;

    Shader(ShaderType type, const std::string& filePath, bool isPath = true) : shaderType(type) {
        this->id = glCreateShader(shaderType);

        auto source = isPath ? getFileContents(filePath) : filePath;
        const char * data = source.c_str();
        glShaderSource(this->id, 1, &data, nullptr);
        glCompileShader(this->id);

        int compiled;
        glGetShaderiv(this->id, GL_COMPILE_STATUS, &compiled);
        if (compiled) return;

        glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &compiled);
        char* infoLog = (char*)malloc(compiled);
        glGetShaderInfoLog(this->id, compiled, &compiled, infoLog);
        printf("Shader Compilation Error:\n%s\n", infoLog);
        free(infoLog);
    }

    GLuint getId() const {
        return this->id;
    }

    virtual ~Shader() {
        glDeleteShader(this->id);
        this->id = -1;
    }
};

class Program {
private:
    static GLuint lastID;
    GLuint id;

public:
    explicit Program(const std::vector<Shader*>& shaders) {
        this->id = glCreateProgram();
        for (const auto &shader : shaders)
            glAttachShader(this->id, shader->getId());
        glLinkProgram(this->id);
        for (const auto &shader : shaders) {
            glDetachShader(this->id, shader->getId());
            delete shader;
        }

        int linked;
        glGetProgramiv(this->id, GL_LINK_STATUS, &linked);
        if (linked) return;

        glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &linked);
        char* infoLog = (char*)malloc(linked);
        glGetProgramInfoLog(this->id, linked, &linked, infoLog);
        printf("Program Link Error:\n%s\n", infoLog);
        free(infoLog);
    }

    void Use() const {
        if (this->id == lastID) return;
        glUseProgram(this->id);
        lastID = this->id;
    }

    int GetUniformLocation(const std::string& name) const {
        return glGetUniformLocation(this->id, name.c_str());
    }

    void Upload(int location, const glm::mat4& mat4, bool transpose = false) const {
        glProgramUniformMatrix4fv(this->id, location, 1, transpose, &mat4[0][0]);
    }

    void Upload(const std::string& name, const glm::mat4& mat4, bool transpose = false) const {
        glProgramUniformMatrix4fv(this->id, GetUniformLocation(name), 1, transpose, &mat4[0][0]);
    }

    UPLOAD_VECTOR(4)

    UPLOAD_VECTOR(3)

    UPLOAD_VECTOR(2)

    void Upload(int location, float x) const {
        glProgramUniform1fv(this->id, location, 1, &x);
    }

    void Upload(const std::string& name, float x) const {
        glProgramUniform1fv(this->id, GetUniformLocation(name), 1, &x);
    }

    void Upload(const std::string& name, int x) const {
        glProgramUniform1iv(this->id, GetUniformLocation(name), 1, &x);
    }

    virtual ~Program() {
        glDeleteProgram(this->id);
        this->id = -1;
    }
};

#endif //GAMEFRAME_SHADERPROGRAM_H
