#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include <GLFW/glfw3.h>
#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), angle(0.0f), rotSpeed(glm::pi<float>() / 2.0f), plane(79.4f, 53.2f, 100, 100), sky(100.0f) {
	C1Mesh = ObjMesh::load("media/C1-10P_obj.obj", true);
	LightsaberMesh = ObjMesh::load("media/Lightsaber_01.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
	prog.use();
	glEnable(GL_DEPTH_TEST);

	model = mat4(1.0f);
	view = glm::lookAt(vec3(5.0f, 7.5f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	//view = glm::lookAt(camPos, camPos + camFront, camUp);

	GLFWwindow* win = glfwGetCurrentContext();
	if (win)
	{
		// Capture cursor
		glfwSetInputMode(win, GLFW_CURSOR, cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

		glfwGetCursorPos(win, &lastMouseX, &lastMouseY);
		firstMouse = true;
	}

	projection = mat4(1.0f);
	rotateModel = mat4(1.0f);
	rotateModel = glm::translate(rotateModel, vec3(4.0f, 0.26f, 5.0f));
	angle = 0.0f;


	//blinnPhongSpot stuff
	prog.setUniform("Spot.L", vec3(0.7f)); 
	prog.setUniform("Spot.La", vec3(0.5f)); 
	prog.setUniform("Spot.Exponent", 25.0f); 
	prog.setUniform("Spot.Cutoff", glm::radians(20.0f));

	prog.setUniform("Light.L", vec3(0.5f));
	prog.setUniform("Light.La", vec3(0.25f));

	cubeTex = Texture::loadCubeMap("media/texture/cube/space/space");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

	C1diffuseTexture = Texture::loadTexture("media/texture/Chopper_BaseColor.png"); 
	C1normalMap = Texture::loadTexture("media/texture/Chopper_Normal.png"); 

	LSdiffuseTexture = Texture::loadTexture("media/texture/Lightsaber_01_lambert1_BaseColor1.png");
	LSnormalMap = Texture::loadTexture("media/texture/Lightsaber_01_lambert1_Normal.png");

	WorkbenchDiffuseMap = Texture::loadTexture("media/texture/workbench.png");
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();

		skyboxShader.compileShader("shader/skybox.vert");
		skyboxShader.compileShader("shader/skybox.frag");
		skyboxShader.link();
		skyboxShader.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float t)
{
	float deltaT = t - tPrev;
	if (tPrev == 0.0f)
		deltaT = 0.0f;
	tPrev = t;
	angle += 0.25f * deltaT;
	if (angle > glm::two_pi<float>())
		angle -= glm::two_pi<float>();

	GLFWwindow* win = glfwGetCurrentContext();
	if (win)
	{
		if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
			hiltYawDeg -= hiltYawSpeed * deltaT;
		if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) 
			hiltYawDeg += hiltYawSpeed * deltaT;
		if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)   
			hiltYawDeg -= hiltYawSpeed * deltaT; 
		if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS) 
			hiltYawDeg += hiltYawSpeed * deltaT;

		if (hiltYawDeg > 360.0f) hiltYawDeg -= 360.0f;
		if (hiltYawDeg < 0.0f)   hiltYawDeg += 360.0f;
	}
	render();

}

void SceneBasic_Uniform::render()
{

	prog.use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	// BlinnPhong spotlight
	glm::vec4 lightPosWorld = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
	prog.setUniform("Spot.Position", glm::vec3(view * lightPosWorld));

	glm::vec3 targetWorld = glm::vec3(0.0f, 2.0f, 3.0f);
	glm::vec3 lightDirWorldSpot = glm::normalize(targetWorld - glm::vec3(lightPosWorld));

	glm::vec3 lightDirView = glm::mat3(view) * lightDirWorldSpot;
	prog.setUniform("Spot.Direction", lightDirView);
	

	glm::vec3 lightDirWorld = glm::normalize(glm::vec3(-0.6f, -1.0f, -0.4f));
	glm::vec3 lightDirViewSpace = glm::mat3(view) * -lightDirWorld;
	prog.setUniform("Light.Position", glm::vec4(lightDirViewSpace, 0.0f));


	// --- chopper to remove once happy with lighting
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, C1diffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, C1normalMap);

	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.8f));
	prog.setUniform("Material.Shininess", 80.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(2.0f));
	setMatrices();
	C1Mesh->render();


	// Lightsaber hilt
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, LSdiffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, LSnormalMap);

	prog.setUniform("Material.Ks", vec3(0.25f));
	prog.setUniform("Material.Ka", vec3(0.15f));
	prog.setUniform("Material.Shininess", 80.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(-1.0f, 2.0f, 2.0f));
	model = glm::scale(model, vec3(1.5f));
	model = glm::rotate(model, glm::radians(270.0f), vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(300.0f), vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(hiltYawDeg), vec3(0.0f, 1.0f, 0.0f));

	setMatrices();
	LightsaberMesh->render();


	// Workbench
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, WorkbenchDiffuseMap);

	prog.setUniform("Material.Ks", vec3(0.0f));
	prog.setUniform("Material.Ka", vec3(0.1f));
	prog.setUniform("Material.Shininess", 180.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(25.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(0.5f));
	setMatrices();
	plane.render();

	// Skybox stuff
	skyboxShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	skyboxShader.setUniform("SkyboxTex", 0);

	glm::mat4 skyView = glm::mat4(glm::mat3(view));
	glm::mat4 skyMVP = projection * skyView;
	skyboxShader.setUniform("MVP", skyMVP);
	sky.render();
}

void SceneBasic_Uniform::setMatrices() {
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
    height = h;
	projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

