#pragma once

#include "core.h"

namespace GRender::shader::internal {

static inline void CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram, const std::string& msg) {
    int success = 0;
    if (isProgram) { glGetProgramiv(shader, flag, &success); }
    else           { glGetShaderiv(shader, flag, &success); }

    if (success == GL_FALSE) {
        char error[1024] = { 0 }; // arbitrary large size
        if (isProgram) { glGetProgramInfoLog(shader, sizeof(error), NULL, error); }
        else           { glGetShaderInfoLog(shader, sizeof(error), NULL, error); }

        ASSERT(false, msg + " :: " + std::string(error));
    }
}

static inline uint32_t CreateShader(const fs::path& shaderPath, GLenum shaderType) {
    // Importing file into stream
    std::ifstream arq(shaderPath);
    std::stringstream strData;
    strData << arq.rdbuf();
    arq.close();

    // Creating shader from data
    uint32_t shader = glCreateShader(shaderType);
    ASSERT(shader != 0, "Failed to create shader!");

    std::string data = strData.str();
    GLint length = (GLint)data.size();
    const GLchar* ptr = data.c_str();
    glShaderSource(shader, 1, &ptr, &length);
    glCompileShader(shader);

    std::string type;
    switch (shaderType) {
    case GL_VERTEX_SHADER:   type = "GL_VERTEX_SHADER"; break;
    case GL_FRAGMENT_SHADER: type = "GL_FRAGMENT_SHADER"; break;
    default:                 type = "GL_COMPUTE_SHADER"; break;
    }

    std::string error = "Shader compilation failed :: " + type + " => " + shaderPath.string();
    CheckShaderError(shader, GL_COMPILE_STATUS, false, error);

    return shader;
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template <typename TP>
inline void setUniform(uint32_t programID, const std::string& name, const TP& value) {
    ASSERT(false, "setUniform -> Unsupported type!");
}


// SINGLE VALUES //////////////////////////////////////////
template<>
inline void setUniform(uint32_t programID, const std::string& name, const int32_t& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform1i(loc, val);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const uint32_t& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform1ui(loc, val);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const float& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform1f(loc, val);
}


// VECTORS TWO ////////////////////////////////////////////
template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::ivec2& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform2i(loc, val.x, val.y);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::uvec2& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform2ui(loc, val.x, val.y);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::vec2& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform2f(loc, val.x, val.y);
}


// VECTORS THREE //////////////////////////////////////////
template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::ivec3& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform3i(loc, val.x, val.y, val.z);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::uvec3& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform3ui(loc, val.x, val.y, val.z);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::vec3& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform3f(loc, val.x, val.y, val.z);
}

// VECTORS FOUR ///////////////////////////////////////////
template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::ivec4& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform4i(loc, val.x, val.y, val.z, val.w);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::uvec4& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform4ui(loc, val.x, val.y, val.z, val.w);
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::vec4& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform4f(loc, val.x, val.y, val.z, val.w);
}

// MATRICES ///////////////////////////////////////////////
template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::mat2& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniformMatrix2fv(loc, 1, false, glm::value_ptr(val));
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::mat3& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniformMatrix3fv(loc, 1, false, glm::value_ptr(val));
}

template<>
inline void setUniform(uint32_t programID, const std::string& name, const glm::mat4& val) {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(val));
}

} // namespace GRender::shader::internal
