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

    std::string type = shaderType == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER";
    std::string error = "Shader compilation failed :: " + type + " => " + shaderPath.string();
    CheckShaderError(shader, GL_COMPILE_STATUS, false, error);

    return shader;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Shader::Shader(const fs::path& vtxPath, const fs::path& frgPath) {
    ASSERT(fs::exists(vtxPath), "Shader not found! => " + vtxPath.string());
    ASSERT(fs::exists(frgPath), "Shader not found! => " + frgPath.string());

    uint32_t vtx = CreateShader(vtxPath, GL_VERTEX_SHADER);
    uint32_t frg = CreateShader(frgPath, GL_FRAGMENT_SHADER);

    // Create program
    programID = glad_glCreateProgram();
    glAttachShader(programID, vtx);
    glAttachShader(programID, frg);

    // Link shaders to program
    glad_glLinkProgram(programID);

    CheckShaderError(programID, GL_LINK_STATUS, true, 
                     "Cannot link shader programs => " + vtxPath.string() + " -- " + frgPath.string());

    glDeleteShader(vtx);
    glDeleteShader(frg);
}

Shader::~Shader(void) {
    glad_glDeleteProgram(programID);
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


void Shader::setTexture(const Texture& tex, uint32_t slot) const {

    tex.bind(slot);

    const std::string name = "texSampler[" + std::to_string(slot) + "]";
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform1i(loc, slot);
}

void Shader::setInteger(const std::string &name, int val) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform1i(loc, val);
}

void Shader::setFloat(const std::string &name, float val) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform1f(loc, val);
}

void Shader::setVec2f(const std::string &name, const float *v) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform2f(loc, v[0], v[1]);
}

void Shader::setVec3f(const std::string &name, const float *v) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform3f(loc, v[0], v[1], v[2]);
}

void Shader::setVec4f(const std::string &name, const float *v) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform4f(loc, v[0], v[1], v[2], v[3]);
}

void Shader::setMatrix3f(const std::string &name, const float *mat) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniformMatrix3fv(loc, 1, GL_FALSE, mat);
}

void Shader::setMatrix4f(const std::string &name, const float *mat) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void Shader::setIntArray(const std::string &name, const int *ptr, int32_t N) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform1iv(loc, N, ptr);
}

void Shader::setFloatArray(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform1fv(loc, N, ptr);
}

void Shader::setVec2fArray(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform2fv(loc, N, ptr);
}

void Shader::setVec3fArray(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniform3fv(loc, N, ptr);
}

void Shader::setMat3Array(const std::string &name, const float *ptr, int32_t N) const {
    int32_t loc = glad_glGetUniformLocation(programID, name.c_str());
    glad_glUniformMatrix3fv(loc, N, true, ptr);
}

} // namespace GRender