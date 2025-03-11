#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint lightningIntensityLoc;


// camera
gps::Camera myCamera(
    glm::vec3(-7.213614, 4.348306, 1.593343),
    glm::vec3(-7.282603, 4.345095, 1.665663), //target 
    glm::vec3(0.0f, 1.0f, 0.0f));


GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D tractor;
GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
gps::Shader depthMapShader;
gps::Shader waterShader;
gps::Shader treeShader;

//mouse 
GLfloat cameraSpeed = 0.1f;

double lastX = 400, lastY = 100;
bool firstMouse = true;
float yaw = -90, pitch = 0;
float sensitivity = 0.01f;

//shadow

glm::mat4 modelTeapotGlobal;
glm::mat4 modelTractorGlobal;


GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;
const unsigned int SHADOW_HEIGHT = 2048;
const unsigned int SHADOW_WIDTH = 2048;



//pointLight
GLfloat is_light = 1.0f; // "ON" 



glm::mat4 lightProjection, lightView, lightSpaceMatrix;
//water lake 
gps::Model3D water; 



//tractor 
glm::vec3 tractorPosition = glm::vec3(-10.619689 , 3.759f  , 1.302042); // init pos 
float tractorRotationAngle = 0.0f; //rotation angle
float tractorSpeed = 0.05f; 

//animation 
float animationDuration = 10.0f; 
float animationTime = 0.0f;     
bool animationActive = true;  
std::vector<glm::vec3> cameraAnimationPath = {
    {-6.948321, 4.721837, 1.574531},
    {-7.032304, 4.709858, 1.627479},
    {-7.118296, 4.699631, 1.677487},
    {-7.204288, 4.689404, 1.727495},
    {-7.290279, 4.679176, 1.777503},
    {-7.376271, 4.668949, 1.827512},
    {-7.462263, 4.658722, 1.877520},
    {-7.551192, 4.648478, 1.922091},
    {-7.640121, 4.638233, 1.966663},
    {-7.729051, 4.627989, 2.011234},
    {-7.823190, 4.617241, 2.043208},
    {-7.917329, 4.606493, 2.075182},
    {-8.011468, 4.595745, 2.107157},
    {-8.109873, 4.585171, 2.121460},
    {-8.208278, 4.574596, 2.135763},
    {-8.306683, 4.564022, 2.150066},
    {-8.405087, 4.553448, 2.164369},
    {-8.500480, 4.543151, 2.136185},
    {-8.595872, 4.532854, 2.108001},
    {-8.691264, 4.522557, 2.079818},
    {-8.786656, 4.512260, 2.051634},
    {-8.872155, 4.501357, 2.000928},
    {-8.957654, 4.490453, 1.950223},
    {-9.043153, 4.479549, 1.899518},
    {-9.112521, 4.468888, 1.828284},
    {-9.181890, 4.458227, 1.757050},
    {-9.251258, 4.447566, 1.685815},
    {-9.320626, 4.436904, 1.614581},
    {-9.376939, 4.427146, 1.532522},
    {-9.433251, 4.417387, 1.450464},
    {-9.487530, 4.407664, 1.367041},
    {-9.541808, 4.397940, 1.283619},
    {-9.596087, 4.388216, 1.200197},
    {-9.650365, 4.378493, 1.116774},
    {-9.703092, 4.369082, 1.032327},
    {-9.755818, 4.359671, 0.947880},
};
int currentAnimationIndex = 0; 
float animationStepTime = 0.25f; 
float accumulatedTime = 0.0f; 
float lastFrameTime = 0.0f;
float deltaTime = 0.0f;

//lightning
float lightIntensity = 1.0f;
bool isLightning = false;
float lightningDuration = 0.2f; 
float lightningTimer = 0.0f;


GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    ///keycodes -> [0, 1024].
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //initial position 
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    // mouse movement
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; //OpenGL OY is inverted
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset; // Left/Right
    pitch += yoffset; // Up/Down
    // can't reverse the camera
    if (pitch > 89.0f)
        pitch = 89.0f;
    // Gimbal Lock effect 
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    //hides the cursor and fixes it to the window
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void constrainCameraPosition() {
    float y_min = 0.5f; 
    glm::vec3 position = myCamera.getPosition();
    if (position.y < y_min) {
        position.y = y_min;
        myCamera.setPosition(position); 
    }
}


void processMovement() {


    if (pressedKeys[GLFW_KEY_0]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_8]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_9]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_O]) {
        is_light = 1.0f; // 'ON' pointLight 
        myBasicShader.useShaderProgram();
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "is_light"), is_light);
    }

    if (pressedKeys[GLFW_KEY_P]) {
        is_light = 0.0f; // 'OFF' pointLight 
        myBasicShader.useShaderProgram();
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "is_light"), is_light);
    }

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model)); // local space -> view space.
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    constrainCameraPosition();
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    //tractor movement 
    glm::vec3 forwardDirection = glm::vec3(
        sin(glm::radians(tractorRotationAngle)), // X
        0.0f,                                   // Y
        cos(glm::radians(tractorRotationAngle)) // Z
    );

    if (pressedKeys[GLFW_KEY_UP]) {
        tractorPosition += forwardDirection * tractorSpeed;
    }
    if (pressedKeys[GLFW_KEY_DOWN]) {
        tractorPosition -= forwardDirection * tractorSpeed;
    }

    if (pressedKeys[GLFW_KEY_LEFT]) {
        tractorRotationAngle += 0.1f; 
    }
    if (pressedKeys[GLFW_KEY_RIGHT]) {
        tractorRotationAngle -= 0.1f; 
    }

    //lightning
    if (pressedKeys[GLFW_KEY_L]) {
        isLightning = true;
        lightningTimer = lightningDuration;
        myBasicShader.useShaderProgram();
        glUniform1f(lightningIntensityLoc, 3.0f);
    }

}

void initOpenGLWindow() {
    myWindow.Create(1920, 1080, "Welcome to my village");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
 
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing

    glEnable(GL_BLEND); //alpha-blending 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    //glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    teapot.LoadModel("models/scene/myVillage.obj");
    tractor.LoadModel("models/scene/tractor.obj");
    water.LoadModel("models/lakeWater.obj" , "models/");
}

void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    waterShader.loadShader("shaders/water.vert", "shaders/water.frag");
    treeShader.loadShader("shaders/tree.vert", "shaders/tree.frag");

}
/*
//MODEL -> where the object is and how it is oriented 
//VIEW ->  position and orientation of the camera -> how the object is seen from the camera's perspective)
*/
void initUniforms() {
    myBasicShader.useShaderProgram();

    // model
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // cuurent view 
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // view -> shader 
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix)); //???????

   
    projection = glm::perspective(glm::radians(45.0f), //FIELF OF VIEW 
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, //aspect ratio
        0.1f, 20.0f); // N / F plane (view volume limits) frustum
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
  
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

   
    lightDir = glm::vec3(0.5f, 15.0f, 0.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
  
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

 
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //pointLight 1
    glm::vec3 pointLightPosition1 = glm::vec3(-3.0f, 2.0f, -3.0f);
    glm::vec3 pointLightColor1 = glm::vec3(1.0f, 0.5f, 0.5f);
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosition1"), 1, glm::value_ptr(pointLightPosition1));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor1"), 1, glm::value_ptr(pointLightColor1));

    // ointlight 2
    glm::vec3 pointLightPosition2 = glm::vec3(3.0f, 1.0f, 3.0f);
    glm::vec3 pointLightColor2 = glm::vec3(0.5f, 0.5f, 1.0f);
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosition2"), 1, glm::value_ptr(pointLightPosition2));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor2"), 1, glm::value_ptr(pointLightColor2));

    //atenuare 
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "constant"), constant);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "linear"), linear);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "quadratic"), quadratic);

    //fog 

    float fogDensityValue = 0.2f; // can be modified for a lowe/higher value 
    GLint fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1f(fogDensityLoc, fogDensityValue); //send fogdensity to shader 

    //shadow 
    depthMapShader.useShaderProgram();
    GLint lightSpaceMatrixLoc = glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix");
    glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    GLint shadowMapLoc = glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap");
    glUniform1i(shadowMapLoc, 3);


    //skybox
    skyboxShader.useShaderProgram();

    glm::mat4 view = glm::mat4(glm::mat3(myCamera.getViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //lightning 
    lightningIntensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightningIntensity");
    if (lightningIntensityLoc != -1) {
        glUniform1f(lightningIntensityLoc, 1.0f); // normal value (by default)
    }

}

void setupLightSpaceMatrix() {
    const float near_plane = 1.0f, far_plane = 30.0f;
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

    // view Matrix for light 
    glm::vec3 lightPos = glm::vec3(0.5f, 15.0f, 0.0f); // same position as lightDir
    glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    lightView = glm::lookAt(lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));

    lightSpaceMatrix = lightProjection * lightView;
}

void renderSceneObject(gps::Shader shader) {
    shader.useShaderProgram();

    // model to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    if (modelLoc == -1) {
        std::cerr << "Error: Uniform 'model' not found!" << std::endl;
    }
 
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    if (normalMatrixLoc == -1) {
        std::cerr << "Error: Uniform 'normalMatrix' not found!" << std::endl;
    }

    // desenare scena
    std::cerr << "Drawing scene object...\n";
    teapot.Draw(shader);
    glCheckError();
}

void renderDepthMap(gps::Shader& depthShader) {
    depthShader.useShaderProgram();

  
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    
    glCullFace(GL_FRONT);

    GLint lightSpaceMatrixLoc = glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix");
    glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "model"),
        1, GL_FALSE,
        glm::value_ptr(modelTeapotGlobal));
    teapot.Draw(depthShader);

    glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "model"),
        1, GL_FALSE,
        glm::value_ptr(modelTractorGlobal));
    tractor.Draw(depthShader);

    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]); //original 
}



void initSkyBox() {
    std::vector<const GLchar*> faces;

    // SkyBox textures
    faces.push_back("models/scene/px.tga");
    faces.push_back("models/scene/nx.tga");
    faces.push_back("models/scene/py.tga");
    faces.push_back("models/scene/ny.tga");
    faces.push_back("models/scene/pz.tga");
    faces.push_back("models/scene/nz.tga");

    mySkyBox.Load(faces);
    if (faces.empty()) {
        std::cerr << "SkyBox faces vector is empty!" << std::endl;
    }

}

void updateScene(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    // Update scene transformations
    modelTeapotGlobal = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelTeapotGlobal = glm::scale(modelTeapotGlobal, glm::vec3(0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelTeapotGlobal));

    // only send normal matrix if not in depth pass
    if (!depthPass) {
        glm::mat3 normalTeapot = glm::mat3(glm::inverseTranspose(view * modelTeapotGlobal));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalTeapot));
    }

    // draw teapot (my scene actually , but it's fine ) 
    teapot.Draw(shader);


    // update tractor transformations
  
    modelTractorGlobal = glm::mat4(1.0f);

    modelTractorGlobal = glm::translate(modelTractorGlobal, tractorPosition);
    modelTractorGlobal = glm::rotate(modelTractorGlobal, glm::radians(tractorRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelTractorGlobal = glm::scale(modelTractorGlobal, glm::vec3(0.05f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelTractorGlobal));

    if (!depthPass) {
        glm::mat3 normalTractor = glm::mat3(glm::inverseTranspose(view * modelTractorGlobal));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalTractor));
    }

    // randare tractor
    tractor.Draw(shader);


    // only process additional lighting and effects if not in depth pass
    if (!depthPass) {
        //update point light positions in eye space
        glm::vec3 pointLightPosWorld1 = glm::vec3(-3.0f, 2.0f, -3.0f);
        glm::vec3 pointLightPosEye1 = glm::vec3(view * glm::vec4(pointLightPosWorld1, 1.0f));
        glUniform3fv(glGetUniformLocation(shader.shaderProgram, "pointLightPosition1"),
            1, glm::value_ptr(pointLightPosEye1));

        glm::vec3 pointLightPosWorld2 = glm::vec3(3.0f, 1.0f, 3.0f);
        glm::vec3 pointLightPosEye2 = glm::vec3(view * glm::vec4(pointLightPosWorld2, 1.0f));
        glUniform3fv(glGetUniformLocation(shader.shaderProgram, "pointLightPosition2"),
            1, glm::value_ptr(pointLightPosEye2));

        // update light state
        glUniform1f(glGetUniformLocation(shader.shaderProgram, "is_light"), is_light);
        
    }
}
GLuint waterDiffuseTexture;

void initWaterTexture() {
    glGenTextures(1, &waterDiffuseTexture);
    glBindTexture(GL_TEXTURE_2D, waterDiffuseTexture);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/lakeWater.jpeg", &width, &height, &nrChannels, 0);

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void renderWater() {
    waterShader.useShaderProgram();

    // model matrix for water
    glm::mat4 modelWater = glm::mat4(1.0f);
    modelWater = glm::scale(modelWater, glm::vec3(5.0f)); 
    modelWater = glm::translate(modelWater, glm::vec3(0.0f, 0.1f, 0.0f)); 

    GLint modelLoc = glGetUniformLocation(waterShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelWater));

    GLint viewLoc = glGetUniformLocation(waterShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    GLint projectionLoc = glGetUniformLocation(waterShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

 
    glm::mat3 normalMatrixWater = glm::mat3(glm::inverseTranspose(view * modelWater));
    GLint normalMatrixLoc = glGetUniformLocation(waterShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixWater));

    glDisable(GL_CULL_FACE);
    water.Draw(waterShader);
    glEnable(GL_CULL_FACE);

}

float generateRandomIntensity() {
    return 2.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (3.5f - 2.5f)));
}


void renderScene() {
    //render depth map
    depthMapShader.useShaderProgram();

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT); //off 

    //send lightSpace to shader
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    updateScene(depthMapShader, true);

    glCullFace(GL_BACK); //on 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // shadow texture 
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

    //send lightSpace to shader
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
        1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    updateScene(myBasicShader, false); //with shadows (opac) 

    waterShader.useShaderProgram();
    water.Draw(waterShader);

    skyboxShader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, view, projection);
    // lightning 
    if (isLightning) {
        if (lightningTimer > 0.1f) {
            lightIntensity = generateRandomIntensity();
        }
        else {
            lightIntensity = 1.0f; //by default value 
        }
        lightningTimer -= deltaTime;
        if (lightningTimer <= 0.0f) {
            isLightning = false;
            lightIntensity = 1.0f;
        }
    }


    // update uniform 
    myBasicShader.useShaderProgram();
    glUniform1f(lightningIntensityLoc, lightIntensity);


    myBasicShader.useShaderProgram();
    GLint lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3f(lightColorLoc, lightColor.r * lightIntensity, lightColor.g * lightIntensity, lightColor.b * lightIntensity);


}

//  shadow mapping
void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);

    // Creare textură pentru depth map
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    //  NEAREST for shadow mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void animateCamera(float deltaTime) {
    if (!animationActive) return;

    if (currentAnimationIndex >= cameraAnimationPath.size() - 1) {
        animationActive = false;
        return;
    }

    accumulatedTime += deltaTime;

    if (accumulatedTime >= animationStepTime) {
        accumulatedTime -= animationStepTime;
        currentAnimationIndex++;
        if (currentAnimationIndex >= cameraAnimationPath.size() - 1) {
            animationActive = false;
            return;
        }
    }

    glm::vec3 startPosition = cameraAnimationPath[currentAnimationIndex];
    glm::vec3 endPosition = cameraAnimationPath[currentAnimationIndex + 1];

    float t = accumulatedTime / animationStepTime;
    glm::vec3 interpolatedPosition = glm::mix(startPosition, endPosition, t);

    // Constraint
    float y_min = 0.5f; 
    if (interpolatedPosition.y < y_min) {
        interpolatedPosition.y = y_min;
    }

    myCamera.setPosition(interpolatedPosition);
    glm::vec3 targetPosition = glm::normalize(endPosition - startPosition) + interpolatedPosition;
    myCamera.setTarget(targetPosition);

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}





void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {
    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initFBO();
    initSkyBox();
    initUniforms();
    setWindowCallbacks();

    //glCheckError();

    // animation
    animationActive = true;
    currentAnimationIndex = 0;
    accumulatedTime = 0.0f;

    // time variables 
    lastFrameTime = glfwGetTime();

  
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        if (animationActive) {
            animateCamera(deltaTime);
        }
        else {
            processMovement();
        }

        setupLightSpaceMatrix();  // actualizează matricea de lumina
        renderScene();            // randare scena

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
