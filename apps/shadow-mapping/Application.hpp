#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/load_obj.hpp>

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

    GLuint m_SceneVBO = 0;
    GLuint m_SceneIBO = 0;
    GLuint m_SceneVAO = 0;

    GLuint m_QuadShadingVBO = 0;
    GLuint m_QuadShadingIBO = 0;
    GLuint m_QuadShadingVAO = 0;

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
    GLint m_uKaSampler = -1;
    GLint m_uKsSampler = -1;
    GLint m_uShininessSampler = -1;
    GLint m_uShininess = -1;

    glmlv::GLProgram m_program_shadingPass;
    glmlv::GLProgram m_program_geometryPass;
    glmlv::ViewController m_Camera;

    glmlv::ObjData sponza;
    std::vector<GLuint> sponzaTextures;

    enum GBufferTextureType
    {
        GPosition = 0,
        GNormal,
        GAmbient,
        GDiffuse,
        GGlossyShininess,
        GDepth, // On doit créer une texture de depth mais on écrit pas directement dedans dans le FS. OpenGL le fait pour nous (et l'utilise).
        GBufferTextureCount
    };
    const GLenum m_GBufferTextureFormat[GBufferTextureCount] = { GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGBA32F, GL_DEPTH_COMPONENT32F };

    GLuint m_GBufferTextures[GBufferTextureCount];

    GLuint m_FBO;

    GLint m_uGPosition = -1;
    GLint m_uGNormal = -1;
    GLint m_uGAmbient = -1;
    GLint m_uGDiffuse = -1;
    GLint m_uGlossyShininess = -1;
    GLint m_uGDirectionalLightDirect = -1;
    GLint m_uGDirectionalLightIntensity = -1;
    GLint m_uGPointLightPosition = -1;
    GLint m_uGPointLightIntensity = -1;
    GLint m_uGKd = -1;


    GLuint m_GQuadVBO = 0;
    GLuint m_GQuadIBO = 0;
    GLuint m_GQuadVAO = 0;

    GLuint m_textureSampler = 0;

    GLuint m_directionalSMTexture;
    GLuint m_directionalSMFBO;
    GLuint m_directionalSMSampler;
    int32_t m_nDirectionalSMResolution = 512;


    /** Shadow map **/
    glmlv::GLProgram m_directionalSMProgram;
    GLint m_uDirLightViewProjMatrix;

    float m_DirLightPhiAngleDegrees;
    float m_DirLightThetaAngleDegrees;

    glm::vec3 m_BBoxMax;
    glm::vec3 m_BBoxMin;

    GLint m_uDirLightShadowMapBias = -1;
};
