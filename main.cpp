#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Camera.h"
#include "utils.h"
#include "model.h"

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



    //只对箱子做面剔除，写在了render里面
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);


    //2.Shader
    Shader colorCubeShader = Shader("shader/multiple_light.vs", "shader/multiple_light.fs");
    Shader lightShader = Shader("shader/light.vs", "shader/light.fs");
    Shader frameShader = Shader("shader/frame.vs", "shader/frame.fs");
    Shader transportShader = Shader("shader/transparent.vs", "shader/transparent.fs");
    Shader screenShader = Shader("shader/screen.vs", "shader/screen.fs");
    Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");
    Shader reflectShader("shader/reflect.vs", "shader/reflect.fs");
    Shader refractShader("shader/refract.vs", "shader/refract.fs");


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
    std::vector<float> vertices = ReadVerticesFromFile("vertices/10_box.txt");
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
    std::vector<float> vertices_window = ReadVerticesFromFile("vertices/window.txt");
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


    // ---------------------  screen quad ----------------------------
    std::vector<float> vertices_screen = ReadVerticesFromFile("vertices/quadVertices.txt");
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_screen.size() * sizeof(float), &vertices_screen[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // -------------------- sky box ----------------------------------
    std::vector<float> vertices_sky_box = ReadVerticesFromFile("vertices/skybox.txt");
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_sky_box.size() * sizeof(float),&vertices_sky_box[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // ----------------------------- model --------------------
    Model ourModel("model/nanosuit_reflection/nanosuit.obj");

    //Texture
    unsigned int diffuseMap = loadTexture("photo/container2.png");
    unsigned int specularMap = loadTexture("photo/container2_specular.png");
    unsigned int emissionMap = loadTexture("photo/matrix.jpg");
    unsigned int tranparentMap = loadTexture("photo/blending_transparent_window.png");
    std::vector<std::string> faces
    {
        "photo/skybox/right.jpg",
        "photo/skybox/left.jpg",
        "photo/skybox/top.jpg",
        "photo/skybox/bottom.jpg",
        "photo/skybox/front.jpg",
        "photo/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    
    colorCubeShader.use();
    colorCubeShader.setInt("material.diffuse", 0);
    colorCubeShader.setInt("material.specular", 1);
    colorCubeShader.setInt("material.emission", 2);
    colorCubeShader.setInt("skybox", 3);


    transportShader.use();
    transportShader.setInt("texture1", 0);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    reflectShader.use();
    reflectShader.setInt("skybox", 3);

    refractShader.use();
    refractShader.setInt("skybox", 3);


    // -------------------------- framebuffer ----------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //create a color attachment texture
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    // create a renderbuffer object for depth and stencil attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std:: cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //如何将深度缓冲和模板缓冲附加为一个单独的纹理
    /* glTexImage2D(
            GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
            GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
        );

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);*/

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
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);
        //glStencilFunc(GL_NOTEQUAL, 1, 0xff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        colorCubeShader.setFloatVec3("viewPos", camera.getPosition());
        colorCubeShader.setFloat("material.shininess", 10.0f);
        colorCubeShader.setFloat("emission_move", (1.0 + sin(glfwGetTime()) / 2 + 0.5));

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

        colorCubeShader.setFloatVec3("cameraPos", camera.getPosition());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilMask(0xff);

        glBindVertexArray(colorCubeVAO);
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

        // *** reflect *** 
        reflectShader.use();
        model = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        reflectShader.setMatrix4fv("model", 1, model);
        reflectShader.setMatrix4fv("view", 1, view);
        reflectShader.setMatrix4fv("projection", 1, projection);
        reflectShader.setFloatVec3("cameraPos", camera.getPosition());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        ourModel.Draw(reflectShader);


        // *** refract ***
        //refractShader.use();
        //model = glm::mat4(1.0f);
        //view = camera.GetViewMatrix();
        //projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //refractShader.setMatrix4fv("model", 1, model);
        //refractShader.setMatrix4fv("view", 1, view);
        //refractShader.setMatrix4fv("projection", 1, projection);
        //refractShader.setFloatVec3("cameraPos", camera.getPosition());
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);


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
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glDisable(GL_CULL_FACE);


        //--------------- light ----------------------
        lightShader.use();
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightShader.setMatrix4fv("view", 1, view);
        lightShader.setMatrix4fv("projection", 1, projection);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilMask(0x00);

        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightShader.setMatrix4fv("model",1,model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
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
        glActiveTexture(GL_TEXTURE0);
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
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        // ----------------------- skybox cube -------------------------
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        //view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        skyboxShader.setMatrix4fv("view",1, view);
        skyboxShader.setMatrix4fv("projection",1, projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ------------------------- Framebuffer ---------------------
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);	// use the color attachment texture as the texture of the quad plane
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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
