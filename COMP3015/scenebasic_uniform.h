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
	std::unique_ptr<ObjMesh> LightsaberMesh;
	std::unique_ptr<ObjMesh> BladeMEsh;
    glm::mat4 rotateModel;
	GLuint LSdiffuseTexture, LSnormalMap, LSmixingTexture;
    GLuint WorkbenchDiffuseMap;
    GLuint cubeTex;


    float tPrev = 0.0f;
    float angle = 0.0f;
    float rotSpeed;
	bool bladeOn = false;
	bool fogEnabled = true;
	float fogScale = 1.0f;
	bool rusty = false;

	// Input keys
    bool Q_Pressed = false;
	bool E_Pressed = false;
	bool F_Pressed = false;
	bool R_Pressed = false;
	bool C_Pressed = false;
    
    glm::vec3 bladeColour;
    std::array<glm::vec3, 6> bladeColours = {
        glm::vec3(0.0f, 0.5f, 1.0f),  // BLUE
        glm::vec3(0.0f, 1.0f, 0.0f),   // GREEN
        glm::vec3(1.0f, 1.0f, 0.0f),   // YELLOW
        glm::vec3(1.0f, 0.0f, 0.0f),   // RED
        glm::vec3(1.0f, 0.0f, 1.0f),   // MAGENTA
        glm::vec3(1.0f, 1.0f, 1.0f),    // WHITE
    };
    int bladeColourIndex = 0;
    glm::vec3 fogColour = glm::vec3(0.6f, 0.6f, 0.6f);
    const glm::vec3 fogGrey = glm::vec3(0.6f, 0.6f, 0.6f);
    float skyFogAmount = 0.35f;


    GLSLProgram prog, skyboxShader, bladeEmissive;
    void compile();
    void setMatrices();
    void setMatricesEmiss();
    void updateFogColour();
    void updateCamera(float dt);

    float hiltYawDeg = 0.0f;
    float hiltYawSpeed = 90.0f;


    // Camera stuff (ORBIT)
    glm::vec3 camTarget = glm::vec3(0.0f, 0.75f, 0.0f); // focus point (central model)
    float camDistance = 11.0f;

    // Orbit angles
    float camYawDeg = -45.0f;
    float camPitchDeg = 35.0f;

    // Speeds / feel
    float camYawSpeedDeg = 75.0f;      // slightly slower orbit
    float mouseSensitivity = 0.06f;    // less twitchy

    // Restriction ("rubber band" limits)
    float pitchMinDeg = 15.0f;
    float pitchMaxDeg = 55.0f;

    // Optional horizontal restriction
    bool  limitYaw = false;
    float yawCentreDeg = -45.0f;
    float yawLimitDeg = 35.0f;       // +/- from centre

    // Derived / stored camera position
    glm::vec3 camPos = glm::vec3(5.0f, 7.5f, 7.5f);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Keep these (you already have them below)
    bool firstMouse = true;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool cursorCaptured = true;
    bool escWasDown = false;

    // auto center
    bool autoRecentre = false;
    float yawReturnSpeed = 1.5;
    float pitchReturnSpeed = 1.5f;
    float pitchCentreDeg = -20.0f;



    // --- Edge detect (hilt only) ---
    GLSLProgram hiltMaskProg;
    GLSLProgram edgeProg;

    GLuint hiltFbo = 0;
    GLuint hiltMaskTex = 0;
    GLuint hiltDepthRb = 0;

    GLuint fsTriVao = 0;
    bool edgeEnabled = false;

    // tweakables
    float edgeThreshold = 0.2f;   // higher = fewer edges
    float edgeOpacity = 1.0f;   // 0..1
    float edgeThickness = 1.0f;   // 1 = normal, 2 = thicker
    glm::vec3 edgeColour = glm::vec3(1.0f); // black outline
    void initHiltEdgeFbo(int w, int h);

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    
};

#endif // SCENEBASIC_UNIFORM_H
