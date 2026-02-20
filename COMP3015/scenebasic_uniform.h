#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/cube.h"


class SceneBasic_Uniform : public Scene
{
private:
	glm::mat4 rotationMatrix;
    Torus torus;
    Teapot teapot;
    Plane plane;
    Cube cube;
    std::unique_ptr<ObjMesh> R2Mesh;
	std::unique_ptr<ObjMesh> C1Mesh;    
	//std::unique_ptr<ObjMesh> pigMesh;
    glm::mat4 rotateModel;
    GLuint C1diffuseTexture, C1normalMap;
    GLuint R2diffuseTexture, R2normalMap;


    float tPrev = 0.0f;
    float angle = 0.0f;
    float rotSpeed;

    GLSLProgram prog;
    void compile();
    void setMatrices();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    
};

#endif // SCENEBASIC_UNIFORM_H
