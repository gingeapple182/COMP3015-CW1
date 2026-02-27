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

SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), angle(0.0f), rotSpeed(glm::pi<float>() / 2.0f), plane(100.0f, 100.0f, 100, 100), teapot(14, glm::mat4(1.0f)), torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50), sky(100.0f) {
	R2Mesh = ObjMesh::load("media/Low_Poly_R2D2.obj", true);
	C1Mesh = ObjMesh::load("media/C1-10P_obj.obj", true);
	XWingMesh = ObjMesh::load("media/X-Wing.obj", true);
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
		// Start with cursor captured (FPS-style)
		glfwSetInputMode(win, GLFW_CURSOR, cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

		// Seed mouse position to avoid a huge first delta
		glfwGetCursorPos(win, &lastMouseX, &lastMouseY);
		firstMouse = true;
	}

	projection = mat4(1.0f);
	rotateModel = mat4(1.0f);
	rotateModel = glm::translate(rotateModel, vec3(4.0f, 0.26f, 5.0f));
	angle = 0.0f;
	float x, z;
	/*for (int i = 0; i < 3; i++) {
		std::stringstream name;
		name << "Lights[" << i << "].Position";
		x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
		z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
		prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));

	}*/

	//prog.setUniform("lights[0].L", vec3(0.0f, 0.0f, 0.8f) / 2.0f);
	//prog.setUniform("lights[1].L", vec3(0.0f, 0.8f, 0.0f) / 2.0f);
	//prog.setUniform("lights[2].L", vec3(0.8f, 0.0f, 0.0f) / 2.0f);

	//prog.setUniform("lights[0].La", vec3(0.0f, 0.0f, 0.2f));
	//prog.setUniform("lights[1].La", vec3(0.0f, 0.2f, 0.0f));
	//prog.setUniform("lights[2].La", vec3(0.2f, 0.0f, 0.0f));

	//blinnPhongSpot stuff
	//prog.setUniform("Spot.L", vec3(0.7f)); 
	//prog.setUniform("Spot.La", vec3(0.5f)); 
	//prog.setUniform("Spot.Exponent", 25.0f); 
	//prog.setUniform("Spot.Cutoff", glm::radians(100.0f));

	prog.setUniform("Light.L", vec3(0.5f));
	prog.setUniform("Light.La", vec3(0.25f));

	cubeTex = Texture::loadCubeMap("media/texture/cube/space/space");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

	C1diffuseTexture = Texture::loadTexture("media/texture/Chopper_BaseColor.png"); 
	C1normalMap = Texture::loadTexture("media/texture/Chopper_Normal.png"); 
	R2diffuseTexture = Texture::loadTexture("media/texture/R2_diffuse_green.png"); 
	R2normalMap = Texture::loadTexture("media/texture/R2_normal.png");
	LSdiffuseTexture = Texture::loadTexture("media/texture/Lightsaber_01_lambert1_BaseColor.png");
	LSnormalMap = Texture::loadTexture("media/texture/Lightsaber_01_lambert1_Normal.png");
	XWingDiffuseTexture = Texture::loadTexture("media/texture/xwing_main.png");
	XWingNormalMap = Texture::loadTexture("media/texture/xwing_main_n.png");
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

	/*if (this->m_animate) {
		angle += rotSpeed * deltaT;
		if (angle > glm::two_pi<float>())
			angle -= glm::two_pi<float>();
	}*/
	updateCamera(deltaT);
	render();

}

void SceneBasic_Uniform::render()
{

	prog.use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//blinnPhongSpot stuff
	//vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
	//prog.setUniform("Spot.Position", vec3(view * lightPos));
	//glm::mat3 normalMatrix = glm::mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
	//prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

	//moving light
	//vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
	//prog.setUniform("Light.Position", view * lightPos);
	
	//glm::vec3 lightDirWorld = glm::normalize(glm::vec3(-0.6f, -1.0f, -0.4f)); // tweak this
	//glm::vec3 lightPosWorld = -lightDirWorld * 200.0f; // far away in opposite direction
	//prog.setUniform("Light.Position", view * glm::vec4(lightPosWorld, 0.0f));

	glm::vec3 lightDirWorld = glm::normalize(glm::vec3(-0.6f, -1.0f, -0.4f));
	glm::vec3 lightDirViewSpace = glm::mat3(view) * -lightDirWorld;
	prog.setUniform("Light.Position", glm::vec4(lightDirViewSpace, 0.0f));

	//prog.setUniform("Material.Kd", 1.0f, 0.4f, 0.7f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.5f));
	prog.setUniform("Material.Shininess", 180.0f);
	rotateModel = glm::translate(rotateModel, vec3(-0.9f, 0.0f, -0.9f));

	model = mat4(1.0f);
	model = glm::translate(model, vec3(1.0f, 2.0f, -4.0f));
	model = glm::rotate(model, glm::radians(70.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(3.0f));
	setMatrices();
	cube.render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, XWingDiffuseTexture);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, etaDiffuse2Texture);

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, etaDiffuse3Texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, XWingNormalMap);

	//prog.setUniform("Material.Kd", 0.0f, 0.5f, 1.0f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.8f));
	prog.setUniform("Material.Shininess", 180.0f);

	/*model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 4.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(1.0f));
	setMatrices();
	XWingMesh->render();*/

	// --- X-Wing follows camera ---
	glm::vec3 forward = glm::normalize(camFront);
	glm::vec3 right = glm::normalize(glm::cross(forward, camUp));
	glm::vec3 up = glm::normalize(glm::cross(right, forward));

	// Offsets relative to camera
	float followForward = 8.2f;   // in front of camera
	float followDown = -2.7f;  // below camera (negative = down)
	float followRight = 0.0f;   // sideways

	glm::vec3 xwingPos = camPos + forward * followForward + up * followDown + right * followRight;

	// Build an orientation so the ship points the same way as the camera.
	// NOTE: depending on how the OBJ is authored, you may need a fixed extra rotation after this.
	glm::mat4 shipRot = glm::mat4(1.0f);
	shipRot[0] = glm::vec4(right, 0.0f);
	shipRot[1] = glm::vec4(up, 0.0f);
	shipRot[2] = glm::vec4(-forward, 0.0f); // -forward is common for OpenGL forward axis

	model = glm::mat4(1.0f);
	model = glm::translate(model, xwingPos);
	model *= shipRot;
	model = model * glm::rotate(glm::mat4(1.0f), glm::radians(shipRoll), glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(0.5f));
	setMatrices();
	XWingMesh->render();


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, C1diffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, C1normalMap);

	//prog.setUniform("Material.Kd", 1.0f, 0.5f, 0.0f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.8f));
	prog.setUniform("Material.Shininess", 80.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(2.0f));
	setMatrices();
	C1Mesh->render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, LSdiffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, LSnormalMap);

	//prog.setUniform("Material.Kd", 1.0f, 1.0f, 0.0f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.8f));
	prog.setUniform("Material.Shininess", 80.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(-5.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(270.0f), vec3(1.0f, 0.0f, 0.0f));

	setMatrices();
	LightsaberMesh->render();


	//prog.setUniform("Material.Kd", 0.76f, 0.60f, 0.42f);
	prog.setUniform("Material.Ks", vec3(0.2f));
	prog.setUniform("Material.Ka", vec3(0.2f));
	prog.setUniform("Material.Shininess", 18.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(5.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

	setMatrices();
	torus.render();


	//prog.setUniform("Material.Kd", 0.1f, 0.8f, 0.1f);
	prog.setUniform("Material.Ks", vec3(0.0f));
	prog.setUniform("Material.Ka", vec3(0.1f));
	prog.setUniform("Material.Shininess", 180.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
	setMatrices();
	//plane.render();

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
	projection = glm::perspective(glm::radians(fovCurrent), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::updateCamera(float dt)
{
	GLFWwindow* win = glfwGetCurrentContext();
	if (!win) return;

	// --- ESC behaviour (edge-triggered) ---
	bool escDown = (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS);
	if (escDown && !escWasDown)
	{
		if (cursorCaptured)
		{
			// Release mouse
			cursorCaptured = false;
			glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstMouse = true;
		}
		else
		{
			// Cursor already free -> close program
			glfwSetWindowShouldClose(win, GLFW_TRUE);
		}
	}
	escWasDown = escDown;

	// --- Mouse look (only while captured) ---
	if (cursorCaptured)
	{
		double mx, my;
		glfwGetCursorPos(win, &mx, &my);

		if (firstMouse)
		{
			lastMouseX = mx;
			lastMouseY = my;
			firstMouse = false;
		}

		double dx = mx - lastMouseX;
		double dy = lastMouseY - my; // up = positive pitch

		lastMouseX = mx;
		lastMouseY = my;

		yaw += static_cast<float>(dx) * mouseSensitivity;
		pitch += static_cast<float>(dy) * mouseSensitivity;

		// Clamp pitch to avoid flip
		if (pitch > 89.0f)  pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		// Rebuild camFront from yaw/pitch
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camFront = glm::normalize(front);
	}

	// --- Movement speed ---
	float speed = camSpeed * dt;
	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed *= 2.0f;

	bool boosting = (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	float targetFov = boosting ? boostFov : baseFov;

	// Smooth towards target FOV (frame-rate independent-ish)
	float k = 1.0f - expf(-fovSmoothSpeed * dt);
	fovCurrent = fovCurrent + (targetFov - fovCurrent) * k;

	// Rebuild projection with updated FOV
	projection = glm::perspective(glm::radians(fovCurrent), (float)width / (float)height, 0.3f, 100.0f);

	// --- Free-fly directions (true 6DOF translation) ---
	glm::vec3 forward = camFront;
	if (glm::length(forward) > 0.0001f) forward = glm::normalize(forward);

	glm::vec3 right = glm::cross(forward, camUp);
	if (glm::length(right) > 0.0001f) right = glm::normalize(right);

	// (Optional) re-orthonormalise up so drift doesn’t build up over time
	glm::vec3 up = glm::cross(right, forward);
	if (glm::length(up) > 0.0001f) up = glm::normalize(up);
	float strafe = 0.0f;
	
	// --- WASD translate in camera space ---
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) camPos += forward * speed;
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) camPos -= forward * speed;
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
		camPos -= right * speed;
		strafe -= 1.0f;
	}
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
		camPos += right * speed;
		strafe += 1.0f;
	}

	// Bank opposite sign depending on what feels right. Start with this:
	shipRollTarget = -strafe * maxShipRoll;

	// Smooth roll
	float kroll = 1.0f - expf(-shipRollResponsiveness * dt);
	shipRoll = shipRoll + (shipRollTarget - shipRoll) * kroll;

	// Vertical strafe (camera up)
	if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)        camPos += up * speed;
	if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camPos -= up * speed;

	// Apply view matrix (use the re-orthonormalised up)
	view = glm::lookAt(camPos, camPos + forward, up);
}