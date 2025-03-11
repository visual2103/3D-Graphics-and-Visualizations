#ifndef Shader_hpp
#define Shader_hpp

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace gps {

class Shader
{
public:
    GLuint shaderProgram;
    void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
    void useShaderProgram();
    void setMat4(const std::string& name, const glm::mat4& mat) const;


private:
    std::string readShaderFile(std::string fileName);
    void shaderCompileLog(GLuint shaderId);
    void shaderLinkLog(GLuint shaderProgramId);
};

}

#endif /* Shader_hpp */
