#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/cube.h"
#include "helper/skybox.h"


class SceneBasic_Uniform : public Scene
{
private:
	glm::mat4 rotationMatrix;
    SkyBox sky;
    Torus torus;
    Teapot teapot;
    Plane plane;
    Cube cube;
    std::unique_ptr<ObjMesh> R2Mesh;
	std::unique_ptr<ObjMesh> C1Mesh; 
	std::unique_ptr<ObjMesh> XWingMesh;
	//std::unique_ptr<ObjMesh> pigMesh;
    glm::mat4 rotateModel;
    GLuint C1diffuseTexture, C1normalMap;
    GLuint R2diffuseTexture, R2normalMap;
	GLuint etaDiffuseTexture, etaNormalMap;
	GLuint XWingDiffuseTexture, XWingNormalMap;
    GLuint cubeTex;


    float tPrev = 0.0f;
    float angle = 0.0f;
    float rotSpeed;

    GLSLProgram prog, skyboxShader;
    void compile();
    void setMatrices();

    // Camera stuff
    glm::vec3 camPos = glm::vec3(5.0f, 7.5f, 7.5f);
    glm::vec3 camFront = glm::normalize(glm::vec3(0.0f, 0.75f, 0.0f) - camPos);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float baseFov = 70.0f;
    float boostFov = 82.0f;          // try 78–90
    float fovCurrent = 70.0f;
    float fovSmoothSpeed = 10.0f;    // higher = snappier

    float camSpeed = 6.0f; // units/sec
    void updateCamera(float dt);

    float yaw = -90.0f;      
    float pitch = 0.0f;
    float mouseSensitivity = 0.12f;
    bool firstMouse = true;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    float shipRoll = 0.0f;          // current roll in degrees
    float shipRollTarget = 0.0f;    // target roll in degrees
    float maxShipRoll = 35.0f;      // try 20–45
    float shipRollResponsiveness = 12.0f; // higher = snappier

    bool cursorCaptured = true;
    bool escWasDown = false;

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    
};

#endif // SCENEBASIC_UNIFORM_H
