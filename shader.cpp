#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (geometryPath != nullptr) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //2. compile shaders
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    //geometry shader
    unsigned int geometry;
    if (geometryPath != nullptr) {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "COEMETRY");
    }


    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != nullptr) {
        glAttachShader(ID, geometry);
    }
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr) {
        glDeleteShader(geometry);
    }
}
// activate the shader
void Shader::use()
{
    glUseProgram(ID);
}
// utility uniform functions
void Shader::setBool(const std::string& name, bool value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1i(location, (int)value);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}
void Shader::setInt(const std::string& name, int value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}
void Shader::setFloat(const std::string& name, float value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::setFloatVec3(const std::string& name, const glm::vec3& value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, &value[0]);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::setFloatVec3(const std::string& name, float x, float y, float z) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform3f(location, x,y,z);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::setFloatVec4(const std::string& name, const glm::vec4& value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform4fv(location, 1, &value[0]);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::setFloatVec4(const std::string& name, float x, float y, float z, float w) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform4f(location, x, y, z, w);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::setFloat1Array(const std::string& name, unsigned count, float* value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1fv(location, count, value);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::setFloat4Array(const std::string& name, unsigned count, float* value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform4fv(location, count, value);
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::setMatrix4fv(const std::string& name, unsigned count, glm::mat4 value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value));
    }
    else {
        std::cout << "Fail to find uniform " << name << std::endl;
    }
}

void Shader::bindUniformBlock(const std::string& name, unsigned int bind_point)
{
    unsigned int uniformBlockIndex = glGetUniformBlockIndex(ID, name.c_str());
    if (uniformBlockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(ID, uniformBlockIndex, bind_point);
    }
    else {
        std::cout << "Fail to find uniform block " << name << std::endl;
    }
}



void Shader::checkCompileErrors(unsigned int shader, std::string type) 
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}