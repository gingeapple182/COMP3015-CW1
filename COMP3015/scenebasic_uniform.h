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
#include <array>
#include "helper/cube.h"
#include "helper/skybox.h"


class SceneBasic_Uniform : public Scene
{
private:
	glm::mat4 rotationMatrix;
    SkyBox sky;
    Plane plane;
    Cube cube;
	//std::unique_ptr<ObjMesh> C1Mesh; 
	std::unique_ptr<ObjMesh> LightsaberMesh;
	std::unique_ptr<ObjMesh> BladeMEsh;
    glm::mat4 rotateModel;
    //GLuint C1diffuseTexture, C1normalMap;
	GLuint LSdiffuseTexture, LSnormalMap;
    //GLuint BladeDiffuseTexture;
    GLuint WorkbenchDiffuseMap;
    GLuint cubeTex;


    float tPrev = 0.0f;
    float angle = 0.0f;
    float rotSpeed;
	bool bladeOn = false;
	bool fogEnabled = true;
	float fogScale = 1.0f;
    glm::vec3 fogColour = glm::vec3(1.0f, 0.0f, 1.0f);
    bool Q_Pressed = false;
	bool E_Pressed = false;
	bool F_Pressed = false;
    glm::vec3 bladeColour;
    std::array<glm::vec3, 6> bladeColours = {
        glm::vec3(0.0f, 0.5f, 1.0f),  // BLUE
        glm::vec3(0.0f, 1.0f, 0.0f),   // GREEN
        glm::vec3(1.0f, 0.35f, 0.0f),   // YELLOW
        glm::vec3(1.0f, 0.0f, 0.0f),   // RED
        glm::vec3(1.0f, 0.0f, 1.0f),   // MAGENTA
        glm::vec3(1.0f, 1.0f, 1.0f),    // WHITE
    };
    int bladeColourIndex = 0;



    GLSLProgram prog, skyboxShader, bladeEmissive;
    void compile();
    void setMatrices();
    void setMatricesEmiss();

    // Camera stuff
    glm::vec3 camPos = glm::vec3(5.0f, 7.5f, 7.5f);
    glm::vec3 camFront = glm::normalize(glm::vec3(0.0f, 0.75f, 0.0f) - camPos);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    bool firstMouse = true;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;


    float hiltYawDeg = 0.0f;
    float hiltYawSpeed = 90.0f;
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
