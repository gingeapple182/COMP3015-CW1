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
	//C1Mesh = ObjMesh::load("media/C1-10P_obj.obj", true);
	//LightsaberMesh = ObjMesh::load("media/Lightsaber_01.obj", true);
	LightsaberMesh = ObjMesh::load("media/Lightsaber_03.obj", true);
	BladeMEsh = ObjMesh::load("media/cylinder.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
	prog.use();
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);

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

	//fog
	prog.setUniform("Fog.MinDistance", 10.0f);
	prog.setUniform("Fog.MaxDistance", 45.0f);
	prog.setUniform("Fog.Colour", fogColour);
	

	cubeTex = Texture::loadCubeMap("media/texture/cube/space/space");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);


	LSdiffuseTexture = Texture::loadTexture("media/texture/Lightsaber_03_exp_lambert1_BaseColor1.png");
	LSnormalMap = Texture::loadTexture("media/texture/Lightsaber_03_exp_lambert1_Normal.png");

	//BladeDiffuseTexture = Texture::loadTexture("media/texture/white.jpg");
	WorkbenchDiffuseMap = Texture::loadTexture("media/texture/workbench.png");
	
	skyboxShader.use();
	skyboxShader.setUniform("FogColour", fogColour);
	
	bladeEmissive.use();
	bladeEmissive.setUniform("BladeColour", bladeColours[bladeColourIndex]);
	bladeEmissive.setUniform("Intensity", 8.0f);
	bladeEmissive.setUniform("RimPower", 2.5f);
	bladeEmissive.setUniform("RimIntensity", 0.6f);

	
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();

		bladeEmissive.compileShader("shader/blade.vert");
		bladeEmissive.compileShader("shader/blade.frag");
		bladeEmissive.link();
		bladeEmissive.use();

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
		//blade toggle
		if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS && !Q_Pressed) {
			bladeOn = !bladeOn;   // toggle once
		}
		Q_Pressed = (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS);
		// colour cycling
		if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS && !E_Pressed)
		{
			bladeColourIndex = (bladeColourIndex + 1) % (int)bladeColours.size();

			bladeEmissive.use();
			bladeEmissive.setUniform("BladeColour", bladeColours[bladeColourIndex]);
		}
		E_Pressed = (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS);
		//fog stuff
		if (glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS && !F_Pressed)
		{
			fogEnabled = !fogEnabled;   // toggle once
		}
		F_Pressed = (glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS);
		//rotate hilt
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

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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


	prog.setUniform("FogEnabled", fogEnabled ? 1 : 0);
	prog.setUniform("FogScale", fogScale);

	// Skybox stuff
	skyboxShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	skyboxShader.setUniform("SkyboxTex", 0);

	glm::mat4 skyView = glm::mat4(glm::mat3(view));
	glm::mat4 skyMVP = projection * skyView;
	skyboxShader.setUniform("MVP", skyMVP);

	skyboxShader.setUniform("FogEnabled", fogEnabled ? 1 : 0);
	skyboxShader.setUniform("FogColour", glm::vec3(1.0f, 0.0f, 1.0f)); // or match your Fog.Colour
	skyboxShader.setUniform("SkyFogAmount", 0.35f); // tweak 0.0 -> 1.0

	sky.render();


	

	prog.use();
	//// Workbench
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


	// Blade
	if (bladeOn)
	{
		glm::mat4 bladeBase = mat4(1.0f);
		bladeBase = glm::translate(bladeBase, vec3(10.0f, 2.1f, -4.0f));
		bladeBase = glm::rotate(bladeBase, glm::radians(270.0f), vec3(1.0f, 0.0f, 0.0f));
		bladeBase = glm::rotate(bladeBase, glm::radians(300.0f), vec3(0.0f, 0.0f, 1.0f));

		// -------- Pass 1: White core (writes stencil = 1 where the blade is) --------
		bladeEmissive.use();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		// Stencil: always write 1 where fragments pass depth test
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		model = bladeBase;
		model = glm::scale(model, vec3(0.4f));
		setMatricesEmiss();

		bladeEmissive.setUniform("BladeColour", glm::vec3(1.0f));
		bladeEmissive.setUniform("Intensity", 10.0f);
		bladeEmissive.setUniform("RimPower", 5.0f);
		bladeEmissive.setUniform("RimIntensity", 0.25f);

		bladeEmissive.setUniform("Alpha", 1.0f);
		bladeEmissive.setUniform("CoreAlpha", 1.0f);

		BladeMEsh->render();

		// -------- Pass 2: Coloured glow shell (only OUTSIDE the core silhouette) --------
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		// Don’t write depth for glow, but DO depth test so hilt blocks it
		glDepthMask(GL_FALSE);

		// Stencil: only draw where stencil != 1 (i.e., outside the core)
		glStencilMask(0x00);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// Optional but usually improves the “wrap” look:
		// render only backfaces of the enlarged shell so it appears as an outline
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		model = bladeBase;
		model = glm::scale(model, vec3(0.4f * 1.4f));
		setMatricesEmiss();

		bladeEmissive.setUniform("BladeColour", bladeColours[bladeColourIndex]);
		bladeEmissive.setUniform("Intensity", 2.5f);
		bladeEmissive.setUniform("RimPower", 2.0f);
		bladeEmissive.setUniform("RimIntensity", 1.0f);

		bladeEmissive.setUniform("Alpha", 0.55f);
		bladeEmissive.setUniform("CoreAlpha", 0.0f);

		BladeMEsh->render();

		// -------- Restore state --------
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

	

	
}

void SceneBasic_Uniform::setMatrices() {
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesEmiss() {
	mat4 mv = view * model;
	bladeEmissive.setUniform("ModelViewMatrix", mv);
	bladeEmissive.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	bladeEmissive.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::resize(int w, int h) {
	glViewport(0, 0, w, h);
	width = w;
    height = h;
	projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}