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

using glm::vec3;
using glm::vec4;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : plane(50.0f, 50.0f, 100, 100), teapot(14, glm::mat4(1.0f)), torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50) {
	mesh = ObjMesh::load("media/pig_triangulated.obj",true);
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

	float x, z;
	for (int i = 0; i < 3; i++) {
		std::stringstream name;
		name << "Lights[" << i << "].Position";
		x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
		z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
		prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));

	}

	//prog.setUniform("lights[0].L", vec3(0.0f, 0.0f, 0.8f) / 2.0f);
	//prog.setUniform("lights[1].L", vec3(0.0f, 0.8f, 0.0f) / 2.0f);
	//prog.setUniform("lights[2].L", vec3(0.8f, 0.0f, 0.0f) / 2.0f);

	//prog.setUniform("lights[0].La", vec3(0.0f, 0.0f, 0.2f));
	//prog.setUniform("lights[1].La", vec3(0.0f, 0.2f, 0.0f));
	//prog.setUniform("lights[2].La", vec3(0.2f, 0.0f, 0.0f));

	prog.setUniform("Spot.L", vec3(0.9f));
	prog.setUniform("Spot.La", vec3(0.5f));
	prog.setUniform("Spot.Exponent", 10.0f);
	prog.setUniform("Spot.Cutoff", glm::radians(120.0f));

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
		angle = glm::two_pi<float>();
	render();

}

void SceneBasic_Uniform::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	vec4 lightPos = vec4(10.0f, 10.0f, 10.0f, 1.0f);
	prog.setUniform("Spot.Position", vec3(view * lightPos));
	glm::mat3 normalMatrix = glm::mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
	prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

	prog.setUniform("Material.Kd", 1.0f, 0.5f, 0.0f);
	prog.setUniform("Material.Ks", vec3(0.5f));
	prog.setUniform("Material.Ka", vec3(0.5f));
	prog.setUniform("Material.Shinniness", 180.0f);
	rotateModel = glm::translate(rotateModel, vec3(-0.9f, 0.0f, -0.9f));

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, vec3(3.0f));
	setMatrices();
	mesh->render();


	model = mat4(1.0f);
	model = glm::translate(model, vec3(-4.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(270.0f), vec3(1.0f, 0.0f, 0.0f));

	setMatrices();
	teapot.render();


	model = mat4(1.0f);
	model = glm::translate(model, vec3(4.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

	setMatrices();
	torus.render();


	prog.setUniform("Material.Kd", 0.1f, 0.8f, 0.1f);
	prog.setUniform("Material.Ks", vec3(0.0f));
	prog.setUniform("Material.Ka", vec3(0.1f));
	prog.setUniform("Material.Shinniness", 180.0f);

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

