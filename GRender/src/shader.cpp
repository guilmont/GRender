#include "shader.h"
#include "texture.h"

namespace fs = std::filesystem;

namespace GRender {

static void CheckShaderError(uint32_t shader, uint32_t flag, bool isProgram, const std::string& msg) {
    int success = 0;

    if (isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if (success == GL_FALSE)
    {
        char error[1024] = { 0 }; // arbitrary large size
        if (isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        ASSERT(false, msg + " :: " + std::string(error));
    } 
}

static uint32_t CreateShader(const fs::path& shaderPath, GLenum shaderType) {
    // Importing file into stream
    std::ifstream arq(shaderPath);
    std::stringstream strData;
    strData << arq.rdbuf();
    arq.close();

    // Creating shader from data
    uint32_t shader = glCreateShader(shaderType);
    ASSERT(shader != 0, "Failed to create shader!");

    std::string data = strData.str();
    GLint length = (GLint) data.size();
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

static GLenum convertToGLFormat(texture::Format fmt) {
    switch (fmt) {
    case texture::Format::RGBA8:             return GL_RGBA8;
    case texture::Format::RGBA32:            return GL_RGBA32F;
    case texture::Format::INTEGER:           return GL_R32I;
    case texture::Format::UNSIGNED_INTEGER:  return GL_R32UI;
    case texture::Format::FLOAT:             return GL_R32F;
    default:
        ASSERT(false, "Texture format not supported!!");
        return GL_NONE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Shader::Shader(const fs::path& vtxPath, const fs::path& frgPath) : type(Type::RENDER) {
    ASSERT(fs::exists(vtxPath), "Shader not found! => " + vtxPath.string());
    ASSERT(fs::exists(frgPath), "Shader not found! => " + frgPath.string());

    uint32_t vtx = CreateShader(vtxPath, GL_VERTEX_SHADER);
    uint32_t frg = CreateShader(frgPath, GL_FRAGMENT_SHADER);

    // Create program
    programID = glCreateProgram();
    glAttachShader(programID, vtx);
    glAttachShader(programID, frg);

    // Link shaders to program
    glLinkProgram(programID);

    CheckShaderError(programID, GL_LINK_STATUS, true, 
                     "Cannot link shader programs => " + vtxPath.string() + " -- " + frgPath.string());

    glDeleteShader(vtx);
    glDeleteShader(frg);
}

Shader::Shader(const fs::path& computePath) : type(Type::COMPUTE) {
    ASSERT(fs::exists(computePath), "Compute shader not found! => " + computePath.string());
    uint32_t cmp = CreateShader(computePath, GL_COMPUTE_SHADER);

    programID = glCreateProgram();
    glAttachShader(programID, cmp);
    glLinkProgram(programID);

    glDeleteShader(cmp);
}

Shader::~Shader(void) {
    glDeleteProgram(programID);
}

Shader::Shader(Shader&& shader) noexcept {
    std::swap(programID, shader.programID);
}

Shader& Shader::operator=(Shader&& shader) noexcept {
    if (this != &shader) {
        this->~Shader();
        new (this) Shader(std::move(shader));
    }
    return *this;
}

const Shader& Shader::bind() {
    glUseProgram(programID);
    return *this;
}

void Shader::dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) const {
    ASSERT(type == Type::COMPUTE, "Trying to dispatch a rendering shader!!");
    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Shader::setTexture(const Texture& tex, uint32_t slot) const {
    tex.bind(slot);

    if (type == Type::RENDER) {
        const std::string name = "texSampler[" + std::to_string(slot) + "]";
        int32_t loc = glGetUniformLocation(programID, name.c_str());
        glUniform1i(loc, slot);
    }
    else {
        GLenum fmt = convertToGLFormat(tex.specification().fmt);
        glBindImageTexture(slot, tex.id(), 0, GL_FALSE, 0, GL_READ_WRITE, fmt);
    }
}

void Shader::setInteger(const std::string &name, int val) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform1i(loc, val);
}

void Shader::setFloat(const std::string &name, float val) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform1f(loc, val);
}

void Shader::setVec2f(const std::string &name, const float *v) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform2f(loc, v[0], v[1]);
}

void Shader::setVec3f(const std::string &name, const float *v) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform3f(loc, v[0], v[1], v[2]);
}

void Shader::setVec4f(const std::string &name, const float *v) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform4f(loc, v[0], v[1], v[2], v[3]);
}

void Shader::setMatrix3f(const std::string &name, const float *mat) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, mat);
}

void Shader::setMatrix4f(const std::string &name, const float *mat) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void Shader::setIntArray(const std::string &name, const int *ptr, int32_t N) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform1iv(loc, N, ptr);
}

void Shader::setFloatArray(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform1fv(loc, N, ptr);
}

void Shader::setVec2fArray(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform2fv(loc, N, ptr);
}

void Shader::setVec3fArray(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniform3fv(loc, N, ptr);
}

void Shader::setMat3Array(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glGetUniformLocation(programID, name.c_str());
    glUniformMatrix3fv(loc, N, true, ptr);
}

} // namespace GRender