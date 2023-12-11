#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Camera.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

std::vector<float> ReadVerticesFromFile(const std::string& filename);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camPos = glm::vec3(-1.0f, 0.0f, 5.0f);
glm::vec3 camTarget = glm::vec3(0.6f, 0.7f, 1.3f);
glm::vec3 camFront = glm::normalize(camTarget - camPos);
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera(camPos,camFront,worldUp);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
	//1.init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
std::cout << "Failed to create GLFW window" << std::endl;
glfwTerminate();
return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
std::cout << "Failed to initialize GLAD" << std::endl;
return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //只对箱子做面剔除，写在了render里面
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);


    //2.Shader
    Shader colorCubeShader = Shader("multiple_light.vs", "multiple_light.fs");
    Shader lightShader = Shader("light.vs", "light.fs");
    Shader frameShader = Shader("frame.vs", "frame.fs");
    Shader transportShader = Shader("transparent.vs", "transparent.fs");

    //3.data
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    // transparent window locations
// --------------------------------
    std::vector<glm::vec3> windows
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3(1.5f, 0.0f, 0.51f),
        glm::vec3(0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3(0.5f, 0.0f, -0.6f)
    };
    // ---------------------- cube -------------------------
    std::vector<float> vertices = ReadVerticesFromFile("10_box.txt");
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    unsigned int VBO, colorCubeVAO;
    glGenVertexArrays(1, &colorCubeVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(colorCubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // ------------------------ window ----------------------------
    //绘制窗户的数据
    std::vector<float> vertices_window = ReadVerticesFromFile("window.txt");
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_window.size() * sizeof(float), &vertices_window[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    //Texture
    unsigned int diffuseMap = loadTexture("container2.png");
    unsigned int specularMap = loadTexture("container2_specular.png");
    unsigned int emissionMap = loadTexture("matrix.jpg");
    unsigned int tranparentMap = loadTexture("blending_transparent_window.png");

    colorCubeShader.use();
    colorCubeShader.setInt("material.diffuse", 0);
    colorCubeShader.setInt("material.specular", 1);
    colorCubeShader.setInt("material.emission", 2);

    transportShader.use();
    transportShader.setInt("texture1", 3);

    //4.render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        //clear前把mask设置回0xff!!!!!!!
        glStencilMask(0xff);
        //glClearStencil(0);也需要把掩码设置成0xff
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        //---------------------- colorCube ------------------------------------
        colorCubeShader.use();

        //file:colorCube.fs/vs
        //colorCubeShader.setFloatVec3("light.position", lightPos);
        //colorCubeShader.setFloatVec3("light.position", camera.getPosition());
        colorCubeShader.setFloatVec3("viewPos", camera.getPosition());
        //colorCubeShader.setFloatVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        //colorCubeShader.setFloatVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        //colorCubeShader.setFloatVec3("light.specular", 1.0f, 1.0f, 1.0f);
        colorCubeShader.setFloat("material.shininess", 10.0f);
        colorCubeShader.setFloat("emission_move", (1.0 + sin(glfwGetTime()) / 2 + 0.5));
        //colorCubeShader.setFloat("light.constant", 1.0f);
        //colorCubeShader.setFloat("light.linear", 0.09f);
        //colorCubeShader.setFloat("light.quadratic", 0.032f);
        //colorCubeShader.setFloatVec3("light.direction", camera.getFront());
        //colorCubeShader.setFloat("light.cutOff", glm::cos(glm::radians(5.5f)));
        //colorCubeShader.setFloat("light.outCutOff", glm::cos(glm::radians(6.5f)));

        //file:multiple_light.fs/vs
        // directional light
        colorCubeShader.setFloatVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        colorCubeShader.setFloatVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        colorCubeShader.setFloatVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        colorCubeShader.setFloatVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        // point light 1
        colorCubeShader.setFloatVec3("pointLights[0].position", pointLightPositions[0]);
        colorCubeShader.setFloatVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        colorCubeShader.setFloatVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        colorCubeShader.setFloatVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        colorCubeShader.setFloat("pointLights[0].constant", 1.0f);
        colorCubeShader.setFloat("pointLights[0].linear", 0.09f);
        colorCubeShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        colorCubeShader.setFloatVec3("pointLights[1].position", pointLightPositions[1]);
        colorCubeShader.setFloatVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        colorCubeShader.setFloatVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        colorCubeShader.setFloatVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        colorCubeShader.setFloat("pointLights[1].constant", 1.0f);
        colorCubeShader.setFloat("pointLights[1].linear", 0.09f);
        colorCubeShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        colorCubeShader.setFloatVec3("pointLights[2].position", pointLightPositions[2]);
        colorCubeShader.setFloatVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        colorCubeShader.setFloatVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        colorCubeShader.setFloatVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        colorCubeShader.setFloat("pointLights[2].constant", 1.0f);
        colorCubeShader.setFloat("pointLights[2].linear", 0.09f);
        colorCubeShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        colorCubeShader.setFloatVec3("pointLights[3].position", pointLightPositions[3]);
        colorCubeShader.setFloatVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        colorCubeShader.setFloatVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);

        colorCubeShader.setFloatVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        colorCubeShader.setFloat("pointLights[3].constant", 1.0f);
        colorCubeShader.setFloat("pointLights[3].linear", 0.09f);
        colorCubeShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        colorCubeShader.setFloatVec3("spotLight.position", camera.getPosition());
        colorCubeShader.setFloatVec3("spotLight.direction", camera.getFront());
        colorCubeShader.setFloatVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        colorCubeShader.setFloatVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        colorCubeShader.setFloatVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        colorCubeShader.setFloat("spotLight.constant", 1.0f);
        colorCubeShader.setFloat("spotLight.linear", 0.09f);
        colorCubeShader.setFloat("spotLight.quadratic", 0.032f);
        colorCubeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        colorCubeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        model = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        colorCubeShader.setMatrix4fv("model", 1, model);
        colorCubeShader.setMatrix4fv("view", 1, view);
        colorCubeShader.setMatrix4fv("projection", 1, projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);

        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilMask(0xff);

        glBindVertexArray(colorCubeVAO);
        //开启面剔除会变得很抽象，因为这里修改物理的位置是通过乘model,view,projection三个矩阵，物体的位置点没有变化
        // 面剔除似乎不能正确应对
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        //glFrontFace(GL_CW);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5));
            colorCubeShader.setMatrix4fv("model", 1, model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glDisable(GL_CULL_FACE);


        // ------------------ frame ---------------------
        frameShader.use();
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        frameShader.setMatrix4fv("view", 1, view);
        frameShader.setMatrix4fv("projection", 1, projection);

        glStencilFunc(GL_NOTEQUAL, 1, 0xff);
        glStencilMask(0x00);
        //glDisable(GL_DEPTH_TEST);

        float scale = 1.1f;
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(scale, scale, scale));
            float angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5));
            frameShader.setMatrix4fv("model", 1, model);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glDisable(GL_CULL_FACE);

        //glEnable(GL_DEPTH_TEST);

        //--------------- light ----------------------
        lightShader.use();
        //model = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);
        //lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        //lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;      
        //model = glm::translate(model, lightPos);
        //model = glm::scale(model, glm::vec3(0.2f));
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //lightShader.setMatrix4fv("model", 1, model);
        lightShader.setMatrix4fv("view", 1, view);
        lightShader.setMatrix4fv("projection", 1, projection);
        glBindVertexArray(lightVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilMask(0x00);

        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightShader.setMatrix4fv("model",1,model);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // ---------------- windows (from furthest to nearest) --------------------------
        transportShader.use();
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        transportShader.setMatrix4fv("view", 1, view);
        transportShader.setMatrix4fv("projection", 1, projection);

        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilMask(0x00);
        
        glBindVertexArray(transparentVAO);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, tranparentMap);
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.getPosition() - windows[i]);
            sorted[distance] = windows[i];
        }
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            transportShader.setMatrix4fv("model",1, model);
            //glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    //5.delete
    glDeleteVertexArrays(1, &colorCubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
	return 0;
}

// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyBoard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyBoard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyBoard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyBoard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyBoard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyBoard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset,false);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessScroll(static_cast<float>(yoffset));
}

std::vector<float> ReadVerticesFromFile(const std::string& filename) {
    std::vector<float> vertices;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            float vertex;
            while (iss >> vertex) {
                vertices.push_back(vertex);
            }
        }
        file.close();
    }
    else {
        std::cerr << "Failed to open " << filename << std::endl;
    }

    //std::cout << "size: " << vertices.size() << std::endl;
    //for (auto iter : vertices) {
    //    std::cout << iter << " ";
    //}

    return vertices;
}
