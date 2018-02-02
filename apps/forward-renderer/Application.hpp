#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/ViewController.hpp>

class Application
{
public:
    Application(int argc, char** argv);
	~Application();
    int run();
private:
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;
    
    GLuint m_cubeVBO = 0;
    GLuint m_cubeIBO = 0;
    GLuint m_cubeVAO = 0;

    GLuint m_sphereVBO = 0;
    GLuint m_sphereIBO = 0;
    GLuint m_sphereVAO = 0;
    
    GLint m_uMVMatrix = -1;
    GLint m_uMVPMatrix = -1;
    GLint m_uNormalMatrix = -1;

    // Variables uniformes lumière
    GLint m_uDirectionalLightDirect = -1;
    GLint m_uDirectionalLightIntensity = -1;
    GLint m_uPointLightPosition = -1;
    GLint m_uPointLightIntensity = -1;
    GLint m_uKd = -1;

    // Variable uniforme texture
    GLint m_uKdSampler = -1;

    glmlv::GLProgram m_program;
    glmlv::ViewController m_Camera;

    glmlv::SimpleGeometry cube;
    glmlv::SimpleGeometry sphere;    

    GLuint m_cubeTexture;
    GLuint m_sphereTexture;
};
