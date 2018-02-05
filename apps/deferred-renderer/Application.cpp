#include "Application.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glm/gtc/constants.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    float lightColor[3] = { 1, 1, 1 };
    float lightDirection[3] = { 1, 1, 1 };
    float lightIntensity[3] = { 0.5, 0.5, 0.5 };
    float pointLightIntensity[3] = { 1000, 1000, 1000 };
    float pointLightPosition[3] = { 1, 1, 1 };
    int textureToShow = 0;

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();
        
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code
        const auto sceneDiagonalSize = glm::length(sponza.bboxMax - sponza.bboxMin);
        m_Camera.setSpeed(sceneDiagonalSize * 0.1f); // 10% de la scene parcouru par seconde

        const auto ProjMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f * sceneDiagonalSize, sceneDiagonalSize); // near = 1% de la taille de la scene, far = 100%
        //const auto ProjMatrix = glm::perspective(glm::radians(70.f), float(m_nWindowWidth)/float(m_nWindowHeight), 0.1f, 100.f);
        const auto ViewMatrix = m_Camera.getViewMatrix();

        glUniform3fv(m_uDirectionalLightDirect, 1, glm::value_ptr(ViewMatrix * glm::vec4(lightDirection[0], lightDirection[1], lightDirection[2], 0)));
        glUniform3fv(m_uDirectionalLightIntensity, 1, glm::value_ptr(glm::vec3(lightIntensity[0], lightIntensity[1], lightIntensity[2])));
        glUniform3fv(m_uPointLightPosition, 1, glm::value_ptr(ViewMatrix * glm::vec4(pointLightPosition[0], pointLightPosition[1], pointLightPosition[2], 1)));
        glUniform3fv(m_uPointLightIntensity, 1, glm::value_ptr(glm::vec3(pointLightIntensity[0], pointLightIntensity[1], pointLightIntensity[2])));
        glUniform3fv(m_uKd, 1, glm::value_ptr(glm::vec3(lightColor[0], lightColor[1], lightColor[2])));

        /************************************************************/
        /**** OBJ ****/
        /************************************************************/

        glBindVertexArray(m_SceneVAO);

        {
            const auto MMatrix = glm::mat4();
            const auto MVMatrix = ViewMatrix * MMatrix;
            const auto NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
            glUniformMatrix4fv(m_uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(m_uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glUniformMatrix4fv(m_uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        }

        
        m_program.use();

        auto indexOffset = 0;
        for (int i = 0; i < sponza.shapeCount; ++i)
        {
            const auto materialID = sponza.materialIDPerShape[i];
            glUniform1f(m_uShininess, sponza.materials[materialID].shininess);

            glActiveTexture(GL_TEXTURE0);
            glUniform1i(m_uKdSampler, 0);
            glBindTexture(GL_TEXTURE_2D, sponzaTextures[sponza.materials[materialID].KdTextureId]);

            glActiveTexture(GL_TEXTURE1);
            glUniform1i(m_uKaSampler, 1);
            glBindTexture(GL_TEXTURE_2D, sponzaTextures[sponza.materials[materialID].KaTextureId]);

            glActiveTexture(GL_TEXTURE2);
            glUniform1i(m_uKsSampler, 2);
            glBindTexture(GL_TEXTURE_2D, sponzaTextures[sponza.materials[materialID].KsTextureId]);

            glActiveTexture(GL_TEXTURE3);
            glUniform1i(m_uShininessSampler, 3);
            glBindTexture(GL_TEXTURE_2D, sponzaTextures[sponza.materials[materialID].shininessTextureId]);

            glDrawElements(GL_TRIANGLES, sponza.indexCountPerShape[i], GL_UNSIGNED_INT, (const GLvoid*) (indexOffset * sizeof(GLuint)));
            
            indexOffset += sponza.indexCountPerShape[i];
        }

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + textureToShow);
        
        glBlitFramebuffer(0, 0, m_nWindowWidth, m_nWindowHeight, 0, 0, m_nWindowWidth, m_nWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Colors");
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            ImGui::ColorEdit3("lightColor", lightColor);
            ImGui::Text("Directional Light");
            ImGui::SliderFloat3("lightDirection", &lightDirection[0], -100, 100);
            ImGui::SliderFloat3("lightIntensity", &lightIntensity[0], 0, 10);
            ImGui::Text("Positional Light");
            ImGui::SliderFloat3("pointLightPosition", &pointLightPosition[0], -100, 100);
            ImGui::SliderFloat3("pointLightIntensity", &pointLightIntensity[0], 0, 1000);
            ImGui::Text("Texture To Show");
            ImGui::RadioButton("Position", &textureToShow, 0);
            ImGui::RadioButton("Normal", &textureToShow, 1);
            ImGui::RadioButton("Ambient", &textureToShow, 2);
            ImGui::RadioButton("Diffuse", &textureToShow, 3);
            ImGui::RadioButton("Glossy Shininess", &textureToShow, 4);

            ImGui::End();
        }

        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            m_Camera.update(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" },
    m_Camera(m_GLFWHandle.window(), 5.f)
{
    /************************************************************/
    /**** INITIALISATION ****/
    /************************************************************/

    // Here we load and compile shaders from the library
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "geometryPass.vs.glsl", m_ShadersRootPath / m_AppName / "geometryPass.fs.glsl" });

    m_uMVMatrix = m_program.getUniformLocation("uMVMatrix");
    m_uNormalMatrix = m_program.getUniformLocation("uNormalMatrix");
    m_uMVPMatrix = m_program.getUniformLocation("uMVPMatrix");

    m_uDirectionalLightDirect = m_program.getUniformLocation("uDirectionalLightDirect");
    m_uDirectionalLightIntensity = m_program.getUniformLocation("uDirectionalLightIntensity");
    m_uPointLightPosition = m_program.getUniformLocation("uPointLightPosition");
    m_uPointLightIntensity = m_program.getUniformLocation("uPointLightIntensity");
    m_uKd = m_program.getUniformLocation("uKd");
    m_uKdSampler = m_program.getUniformLocation("uKdSampler");
    m_uKaSampler = m_program.getUniformLocation("uKaSampler");
    m_uKsSampler = m_program.getUniformLocation("uKsSampler");
    m_uShininessSampler = m_program.getUniformLocation("uShininessSampler");
    m_uShininess = m_program.getUniformLocation("uShininess");
    
    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;

    m_program.use();
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    glEnable(GL_DEPTH_TEST);

    /************************************************************/
    /**** OBJ ****/
    /************************************************************/
    glGenBuffers(1, &m_SceneVBO);

    glmlv::loadObj(m_AssetsRootPath / "glmlv" / "models" / "crytek-sponza" / "sponza.obj", sponza);

    sponzaTextures.resize(sponza.textures.size());
    glGenTextures(sponza.textures.size(), sponzaTextures.data());

    glActiveTexture(GL_TEXTURE0);
    for(auto idx = 0; idx < sponza.textures.size(); ++idx) {
        glBindTexture(GL_TEXTURE_2D, sponzaTextures[idx]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, sponza.textures[idx].width(), sponza.textures[idx].height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sponza.textures[idx].width(), sponza.textures[idx].height(), GL_RGBA, GL_UNSIGNED_BYTE, sponza.textures[idx].data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(sponza.textures.size(), m_GBufferTextures);

    // boucle pour init les textures
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
    glGenTextures(GBufferTextureCount, &m_GBufferTextures[0]);
    for(auto i = 0; i < GBufferTextureCount; ++i) {
        glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, m_GBufferTextureFormat[i], m_nWindowWidth, m_nWindowHeight);
    }

    // boucle pour attacher les textures
    for(auto i = 0; i < GBufferTextureCount - 1; ++i) {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_GBufferTextures[i], 0);
    }
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_GBufferTextures[GBufferTextureCount - 1], 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, drawBuffers);
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        exit(0);
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO);
    glBufferStorage(GL_ARRAY_BUFFER, sponza.vertexBuffer.size()*sizeof(sponza.vertexBuffer[0]), sponza.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_SceneIBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SceneIBO);
    glBufferStorage(GL_ARRAY_BUFFER, sponza.indexBuffer.size()*sizeof(sponza.indexBuffer[0]), sponza.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, &m_SceneVAO);
    glBindVertexArray(m_SceneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texCoordsAttrLocation);
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SceneIBO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Application::~Application()
{
    if (m_SceneVBO) {
        glDeleteBuffers(1, &m_SceneVBO);
    }

    if (m_SceneIBO) {
        glDeleteBuffers(1, &m_SceneIBO);
    }

    if (m_SceneVAO) {
        glDeleteBuffers(1, &m_SceneVAO);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
