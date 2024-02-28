#include "Shader.hpp"

namespace gps {

    std::string Shader::citesteShaderContinut(std::string nume)
    {
        std::ifstream descriptorFisier;
        std::stringstream streamFisier;

        descriptorFisier.open(nume);       
        streamFisier << descriptorFisier.rdbuf();
        descriptorFisier.close();

        return streamFisier.str();
    }

    void Shader::compileazaShader(GLuint shaderId) 
    {
        GLint statusSucces;
        GLchar outputCompilare[512];

        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &statusSucces);

        if (statusSucces == false) {
            glGetShaderInfoLog(shaderId, 512, NULL, outputCompilare);
            std::cout << "Shader eroare la compilare\n" << outputCompilare << std::endl;
        }
    }

    void Shader::leagaShader(GLuint shaderProgramId)
    {
        GLint statusSucces;
        GLchar outputLegare[512];

        glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &statusSucces);
        if (statusSucces == false) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, outputLegare);
            std::cout << "Shader eroare la legare\n" << outputLegare << std::endl;
        }
    }

    void Shader::utilizeazaShader() {

        glUseProgram(this->shaderProgram);
    }

    void Shader::incarcaShader(std::string numeVertexShader, std::string numeFragmentShader) 
    {        
        GLuint vertexShader = proceseazaVertexShader(numeVertexShader);
        GLuint fragmentShader = proceseazaFragmentShader(numeFragmentShader);

        this->shaderProgram = glCreateProgram();
        glAttachShader(this->shaderProgram, vertexShader);
        glAttachShader(this->shaderProgram, fragmentShader);
        glLinkProgram(this->shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        leagaShader(this->shaderProgram);
    }

 
   GLuint Shader::proceseazaVertexShader(std::string numeVertexShader)
    {
        std::string v = citesteShaderContinut(numeVertexShader);
        const GLchar* vertexShaderBuff = v.c_str();       
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderBuff, NULL);
        glCompileShader(vertexShader);
        compileazaShader(vertexShader);
        return vertexShader;
    }

   GLuint Shader::proceseazaFragmentShader(std::string numeFragmentShader)
   {
       std::string f = citesteShaderContinut(numeFragmentShader);
       const GLchar* fragmentShaderBuff = f.c_str();
       GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
       glShaderSource(fragmentShader, 1, &fragmentShaderBuff, NULL);
       glCompileShader(fragmentShader);
       compileazaShader(fragmentShader);
       return fragmentShader;
   }
}
