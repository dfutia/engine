#include "shader.h"
#include "Asset/asset.h"

Shader::Shader(const std::string& source, unsigned int type) {
    id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
}

Shader::~Shader() {
    spdlog::info("Shader destroyed");
    glDeleteShader(id);
}

bool Shader::compile() {
    glCompileShader(id);
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    return success;
}

std::string Shader::getInfoLog() const {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> log(length);
    glGetShaderInfoLog(id, length, nullptr, log.data());
    return std::string(log.begin(), log.end());
}

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
}
ShaderProgram::~ShaderProgram() {
    spdlog::info("ShaderProgram destroyed");
    glDeleteProgram(id);
    for (const Shader& shader : shaders) {
        glDeleteShader(shader.id);
    }
}

void ShaderProgram::attach(const Shader& shader) {
    shaders.push_back(shader);
    glAttachShader(id, shader.id);
}

bool ShaderProgram::link() {
    for (Shader& shader : shaders) {
        if (!shader.compile()) {
            std::cerr << "Shader compilation failed: " << shader.getInfoLog() << std::endl;
            return false;
        }
    }

    glLinkProgram(id);
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "Shader program linking failed: " << getInfoLog() << std::endl;
        return false;
    }

    return true;
}

std::string ShaderProgram::getInfoLog() const {
    int length;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> log(length);
    glGetProgramInfoLog(id, length, nullptr, log.data());
    return std::string(log.begin(), log.end());
}

void ShaderProgram::use() const {
    glUseProgram(id);
}

void ShaderProgram::setUniformInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform1i(location, value);
}

void ShaderProgram::setUniformFloat(const std::string& name, float value)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string& name, bool value)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform1i(location, (int)value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
