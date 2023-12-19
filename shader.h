#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    // activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setFloatVec3(const std::string& name,const glm::vec3 &value) const;
    void setFloatVec3(const std::string& name, float x,float y,float z) const;
    void setFloatVec4(const std::string& name, const glm::vec4& value) const;
    void setFloatVec4(const std::string& name, float x, float y, float z, float w) const;
    void setFloat1Array(const std::string& name, unsigned count, float* value) const;
    void setFloat4Array(const std::string& name, unsigned count, float* value) const;
    void setMatrix4fv(const std::string& name, unsigned count, glm::mat4 value) const;

    void bindUniformBlock(const std::string& name,unsigned int bind_point);

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif