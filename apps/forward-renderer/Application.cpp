#include "Application.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/constants.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code
        glm::mat4 ProjMatrix, MVMatrix_Cube, MVMatrix_Sphere, NormalMatrix, ViewMatrix;
        ProjMatrix = glm::perspective(glm::radians(70.f), float(m_nWindowWidth)/float(m_nWindowHeight), 0.1f, 100.f);
        MVMatrix_Cube = glm::translate(glm::mat4(1), glm::vec3(2.,0.,-5.0));
        MVMatrix_Sphere = glm::translate(glm::mat4(1), glm::vec3(-2.,0.,-5.0));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix_Cube));
        //ViewMatrix = viewController.getViewMatrix();
        ViewMatrix = glm::lookAt(glm::vec3(1, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        /**** CUBE ****/
        glBindVertexArray(m_cubeVAO);
        MVMatrix_Cube = ViewMatrix * MVMatrix_Cube;
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix_Cube));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix_Cube));

        glDrawElements(GL_TRIANGLES, cube.indexBuffer.size(), GL_UNSIGNED_INT, nullptr); 
        glBindVertexArray(0);

        /**** SPHERE ****/
        glBindVertexArray(m_sphereVAO);
        MVMatrix_Sphere = ViewMatrix * MVMatrix_Sphere;
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix_Sphere));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix_Sphere));

        glDrawElements(GL_TRIANGLES, sphere.indexBuffer.size(), GL_UNSIGNED_INT, nullptr); 
        glBindVertexArray(0);


        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            ImGui::End();
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            //viewController.update(float(ellapsedTime))
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    /************************************************************/
    /**** INITIALISATION ****/
    /************************************************************/
    // Here we load and compile shaders from the library
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });

    uMVMatrix = m_program.getUniformLocation("uMVMatrix");
    uNormalMatrix = m_program.getUniformLocation("uNormalMatrix");
    uMVPMatrix = m_program.getUniformLocation("uMVPMatrix");
    // const GLint positionAttrLocation = glGetUniformLocation(m_program.glId(), "aVertexPosition");
    // const GLint normalAttrLocation = glGetUniformLocation(m_program.glId(), "aVertexNormal");
    // const GLint texCoordsAttrLocation = glGetUniformLocation(m_program.glId(), "aVertexTexCoords");
    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;

    m_program.use();
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    //viewController = 

    glEnable(GL_DEPTH_TEST);

    /************************************************************/
	/**** CUBE ****/
    /************************************************************/
	glGenBuffers(1, &m_cubeVBO);
	cube = glmlv::makeCube();

	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferStorage(GL_ARRAY_BUFFER, cube.vertexBuffer.size()*sizeof(cube.vertexBuffer[0]), cube.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_cubeIBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeIBO);
    glBufferStorage(GL_ARRAY_BUFFER, cube.indexBuffer.size()*sizeof(cube.indexBuffer[0]), cube.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  
    glGenVertexArrays(1, &m_cubeVAO);

    glBindVertexArray(m_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texCoordsAttrLocation);
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    // "GL_ELEMENT_ARRAY_BUFFER" permet de preciser que ce sont des indexs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /************************************************************/
    /**** SPHERE ****/
    /************************************************************/
    glGenBuffers(1, &m_sphereVBO);
    sphere = glmlv::makeSphere(10);

    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
    glBufferStorage(GL_ARRAY_BUFFER, sphere.vertexBuffer.size()*sizeof(sphere.vertexBuffer[0]), sphere.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_sphereIBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphereIBO);
    glBufferStorage(GL_ARRAY_BUFFER, sphere.indexBuffer.size()*sizeof(sphere.indexBuffer[0]), sphere.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  
    glGenVertexArrays(1, &m_sphereVAO);

    glBindVertexArray(m_sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texCoordsAttrLocation);
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    // "GL_ELEMENT_ARRAY_BUFFER" permet de preciser que ce sont des indexs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

Application::~Application()
{
    if (m_cubeVBO) {
        glDeleteBuffers(1, &m_cubeVBO);
    }

    if (m_cubeIBO) {
        glDeleteBuffers(1, &m_cubeIBO);
    }

    if (m_cubeVAO) {
        glDeleteBuffers(1, &m_cubeVAO);
    }

    if (m_sphereVBO) {
        glDeleteBuffers(1, &m_sphereVBO);
    }

    if (m_sphereIBO) {
        glDeleteBuffers(1, &m_sphereIBO);
    }

    if (m_sphereVAO) {
        glDeleteBuffers(1, &m_sphereVAO);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
