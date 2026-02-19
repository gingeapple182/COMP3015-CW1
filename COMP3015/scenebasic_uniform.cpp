#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), angle(0.0f), rotSpeed(glm::pi<float>() / 2.0f), plane(100.0f, 100.0f, 100, 100), teapot(14, glm::mat4(1.0f)), torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50) {
	R2Mesh = ObjMesh::load("media/Low_Poly_R2D2.obj", true);
	C1Mesh = ObjMesh::load("media/C1-10P_obj.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

	glEnable(GL_DEPTH_TEST);

	model = mat4(1.0f);
	view = glm::lookAt(vec3(5.0f, 7.5f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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

	prog.setUniform("Light.L", vec3(0.9f));
	prog.setUniform("Light.La", vec3(0.05f));
	/*prog.setUniform("Fog.MaxDistance", 25.0f);
	prog.setUniform("Fog.MinDistance", 10.0f);
	prog.setUniform("Fog.Colour", vec3(0.5f, 0.5f, 0.5f));*/

	GLuint diffuseTexture = Texture::loadTexture("media/texture/Chopper_BaseColor.png");
	GLuint normalMap = Texture::loadTexture("media/texture/Chopper_Normal.png");
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
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
	render();

}

void SceneBasic_Uniform::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//blinnPhongSpot stuff
	//vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
	//prog.setUniform("Spot.Position", vec3(view * lightPos));
	//glm::mat3 normalMatrix = glm::mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
	//prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

	vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
	prog.setUniform("Light.Position", view * lightPos);


	prog.setUniform("Material.Kd", 1.0f, 0.4f, 0.7f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.5f));
	prog.setUniform("Material.Shininess", 180.0f);
	rotateModel = glm::translate(rotateModel, vec3(-0.9f, 0.0f, -0.9f));

	model = mat4(1.0f);
	model = glm::translate(model, vec3(1.0f, 2.0f, 0.7f));
	model = glm::rotate(model, glm::radians(70.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(3.0f));
	setMatrices();
	cube.render();


	prog.setUniform("Material.Kd", 0.0f, 0.5f, 1.0f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.8f));
	prog.setUniform("Material.Shininess", 80.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 2.0f, -5.0f));
	model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(0.5f));
	setMatrices();
	R2Mesh->render();


	prog.setUniform("Material.Kd", 1.0f, 0.5f, 0.0f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.8f));
	prog.setUniform("Material.Shininess", 80.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, vec3(2.0f));
	setMatrices();
	C1Mesh->render();


	prog.setUniform("Material.Kd", 1.0f, 1.0f, 0.0f);
	prog.setUniform("Material.Ks", vec3(0.95f));
	prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
	prog.setUniform("Material.Shininess", 180.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(-5.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(270.0f), vec3(1.0f, 0.0f, 0.0f));

	setMatrices();
	teapot.render();


	prog.setUniform("Material.Kd", 0.76f, 0.60f, 0.42f);
	prog.setUniform("Material.Ks", vec3(0.2f));
	prog.setUniform("Material.Ka", vec3(0.2f));
	prog.setUniform("Material.Shininess", 18.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(5.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

	setMatrices();
	torus.render();


	prog.setUniform("Material.Kd", 0.1f, 0.8f, 0.1f);
	prog.setUniform("Material.Ks", vec3(0.0f));
	prog.setUniform("Material.Ka", vec3(0.1f));
	prog.setUniform("Material.Shininess", 180.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
	setMatrices();
	plane.render();
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

