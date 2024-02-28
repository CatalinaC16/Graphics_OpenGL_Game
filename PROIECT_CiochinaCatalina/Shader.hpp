#ifndef Shader_hpp
#define Shader_hpp

#if defined (__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>

namespace gps {

    class Shader {

    public:
        GLuint shaderProgram;
        void incarcaShader(std::string numeVertexShader, std::string numeFragmentShader);
        void utilizeazaShader();

    private:
        std::string citesteShaderContinut(std::string nume);
        void compileazaShader(GLuint shaderId);
        void leagaShader(GLuint shaderProgramId);
        GLuint proceseazaVertexShader(std::string numeVertexShader);
        GLuint proceseazaFragmentShader(std::string numeFragmentShader);
    };

}

#endif /* Shader_hpp */