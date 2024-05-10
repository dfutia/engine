#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct Assets;

struct Shader {
    unsigned int id;

    Shader(const std::string& source, unsigned int type);

    ~Shader();

    bool compile();

    std::string getInfoLog() const;
};

struct ShaderProgram {
    unsigned int id;
    std::vector<std::shared_ptr<Shader>> shaders;

    ShaderProgram();
    ~ShaderProgram();

    void attach(const std::shared_ptr<Shader>& shader);

    bool link();

    std::string getInfoLog() const;

    void use() const;

    void setUniformInt(const std::string& name, int value);

    void setUniformFloat(const std::string& name, float value);

    void setUniform(const std::string& name, bool value);

    void setUniform(const std::string& name, const glm::vec3& value);

    void setUniform(const std::string& name, const glm::mat4& value);
};

#endif 