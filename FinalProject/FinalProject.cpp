/*
Zixuan Zhang Final Project for CS5610 at the UofU
*/

#include <glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "cyTriMesh.h"
#include "cyGL.h"
#include "ObjDrawer.h"
#include "Camera.h"
#include <random>
#include <string>

using namespace cy;

bool flightMode, gBufferMode, ssaoOn;
int prevX, prevY;
float radius = 2;
float angle = 0;
int teapot_num = 8;
std::vector<ObjDrawer*> Scene;
//ObjDrawer* objDrawer, *plane, *teapot2;
Camera* camera;

GLSLProgram SSAO_Geo_prog;
GLSLProgram simple_prog;
GLSLProgram SSAO_prog;
GLSLProgram SSAO_Blur_prog;
GLSLProgram blinn_prog;

//Framebuffers
GLuint gBuffer;
GLuint ssaoFrameBuffer, ssaoBlurFBO;

// Textures
GLuint gPosition, gNormal, gAlbedo;
GLuint ssaoColorTex, ssaoTex;
GLuint noiseTexture;
GLuint rboDepth;

// vector for ssao samples
std::vector<glm::vec3> ssaoKernel;

int scr_w, scr_h;


unsigned int quadVAO = 0;
unsigned int quadVBO, quadCoord;
GLuint pos_loc, texc_loc;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        
			-1.0f,  1.0f, 0.0f, 
			-1.0f, -1.0f, 0.0f, 
			 1.0f,  1.0f, 0.0f, 
			 1.0f, -1.0f, 0.0f
		};
		// texture Coords
		float quadTexCoords[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		pos_loc = glGetAttribLocation(SSAO_prog.GetID(), "m_pos");
		
		glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &quadCoord);
		glBindBuffer(GL_ARRAY_BUFFER, quadCoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexCoords), quadTexCoords, GL_STATIC_DRAW);
		texc_loc = glGetAttribLocation(SSAO_prog.GetID(), "texc");
		glVertexAttribPointer(texc_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	glBindVertexArray(quadVAO);
	glEnableVertexAttribArray(pos_loc);
	glEnableVertexAttribArray(texc_loc);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(pos_loc);
	glDisableVertexAttribArray(texc_loc);
	glBindVertexArray(0);
}

void updateTeapot() {
	for (int i = 0; i < teapot_num; i++) {
		auto o = Scene[i];
		o->setPosition(glm::normalize(glm::vec3(glm::sin(glm::radians(i * 360 / (float)teapot_num)), 0, glm::cos(glm::radians(i * 360 / (float)teapot_num)))) * radius);
		o->setScale(glm::vec3(.05f, .05f, .05f));
		o->setRotation(glm::vec3(-90, 0, (i * 360 / teapot_num) + angle));
	}
}

void drawScene() {
	// geo step
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (auto obj : Scene) {
			obj->setProg(SSAO_Geo_prog.GetID());
			obj->setAttrib();
			obj->draw(camera->getView(), camera->getProj());
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. generate SSAO texture
	//	 ------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFrameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(SSAO_prog.GetID());
		// Send kernel + rotation 
		for (unsigned int i = 0; i < 64; ++i) {
			std::string sample = "samples[" + std::to_string(i) + "]";
			GLint sample_pos = glGetUniformLocation(SSAO_prog.GetID(), sample.c_str());
			glUniform3fv(sample_pos, 1, glm::value_ptr(ssaoKernel[i]));
		}
		GLint p_pos = glGetUniformLocation(SSAO_prog.GetID(), "p");
		glUniformMatrix4fv(p_pos, 1, GL_FALSE, glm::value_ptr(camera->getProj()));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(SSAO_Blur_prog.GetID());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ssaoColorTex);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(blinn_prog.GetID());
	GLuint ssaoOn_loc = glGetUniformLocation(blinn_prog.GetID(), "ssaoOn");
	glUniform1i(ssaoOn_loc, ssaoOn);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ssaoTex);
	renderQuad();
	
}

void initScene() {
	// Setup model-----------------------------------------
	for (int i = 0; i < teapot_num; i++) {
		ObjDrawer* o = new ObjDrawer("res/teapot.obj", false);
		o->setProg(simple_prog.GetID());
		o->setAttrib();
		o->setPosition(glm::normalize(glm::vec3(glm::sin(glm::radians(i * 360/(float)teapot_num)), 0, glm::cos(glm::radians(i * 360/ (float)teapot_num)))) * radius);
		o->setScale(glm::vec3(.05f, .05f, .05f));
		o->setRotation(glm::vec3(-90, 0, i * 360 / teapot_num));
		Scene.push_back(o);
	}
	ObjDrawer* plane = new ObjDrawer("res/plane.obj", false);
	plane->setProg(simple_prog.GetID());
	plane->setAttrib();
	plane->setPosition(glm::vec3(0, 0, 0));
	plane->setScale(glm::vec3(1.f, 1.f, 1.f));
	Scene.push_back(plane);
}

void initShaders() {
	// setup program--------------------------------------------
	GLSLShader sim_vs, sim_fs;
	simple_prog.CreateProgram();
	sim_vs.CompileFile("shaders/SimpleVS.glsl", GL_VERTEX_SHADER);
	sim_fs.CompileFile("shaders/SimpleFS.glsl", GL_FRAGMENT_SHADER);
	simple_prog.AttachShader(sim_vs);
	simple_prog.AttachShader(sim_fs);
	simple_prog.Link();

	GLSLShader SSAO_Geo_vs, SSAO_Geo_fs;
	SSAO_Geo_prog.CreateProgram();
	SSAO_Geo_vs.CompileFile("shaders/ssao_geoVS.glsl", GL_VERTEX_SHADER);
	SSAO_Geo_fs.CompileFile("shaders/ssao_geoFS.glsl", GL_FRAGMENT_SHADER);
	SSAO_Geo_prog.AttachShader(SSAO_Geo_vs);
	SSAO_Geo_prog.AttachShader(SSAO_Geo_fs);
	SSAO_Geo_prog.Link();

	GLSLShader SSAO_vs, SSAO_fs;
	SSAO_prog.CreateProgram();
	SSAO_vs.CompileFile("shaders/ssaoVS.glsl", GL_VERTEX_SHADER);
	SSAO_fs.CompileFile("shaders/ssaoFS.glsl", GL_FRAGMENT_SHADER);
	SSAO_prog.AttachShader(SSAO_vs);
	SSAO_prog.AttachShader(SSAO_fs);
	SSAO_prog.Link();
	glUseProgram(SSAO_prog.GetID());
	GLint sampler_pos = glGetUniformLocation(SSAO_prog.GetID(), "gPosition");
	glUniform1i(sampler_pos, 0);
	sampler_pos = glGetUniformLocation(SSAO_prog.GetID(), "gNormal");
	glUniform1i(sampler_pos, 1);
	sampler_pos = glGetUniformLocation(SSAO_prog.GetID(), "texNoise");
	glUniform1i(sampler_pos, 2);

	GLSLShader SSAO_blur_vs, SSAO_blur_fs;
	SSAO_Blur_prog.CreateProgram();
	SSAO_blur_vs.CompileFile("shaders/ssaoVS.glsl", GL_VERTEX_SHADER);
	SSAO_blur_fs.CompileFile("shaders/BlurFS.glsl", GL_FRAGMENT_SHADER);
	SSAO_Blur_prog.AttachShader(SSAO_blur_vs);
	SSAO_Blur_prog.AttachShader(SSAO_blur_fs);
	SSAO_Blur_prog.Link();
	glUseProgram(SSAO_Blur_prog.GetID());
	sampler_pos = glGetUniformLocation(SSAO_Blur_prog.GetID(), "gPosition");
	glUniform1i(sampler_pos, 0);
	sampler_pos = glGetUniformLocation(SSAO_Blur_prog.GetID(), "gNormal");
	glUniform1i(sampler_pos, 1);
	sampler_pos = glGetUniformLocation(SSAO_Blur_prog.GetID(), "gAlbedo");
	glUniform1i(sampler_pos, 2);
	sampler_pos = glGetUniformLocation(SSAO_Blur_prog.GetID(), "ssaoInput");
	glUniform1i(sampler_pos, 3);

	GLSLShader blinn_vs, blinn_fs;
	blinn_prog.CreateProgram();
	blinn_vs.CompileFile("shaders/ssaoVS.glsl", GL_VERTEX_SHADER);
	blinn_fs.CompileFile("shaders/blinnFS.glsl", GL_FRAGMENT_SHADER);
	blinn_prog.AttachShader(blinn_vs);
	blinn_prog.AttachShader(blinn_fs);
	blinn_prog.Link();
	glUseProgram(blinn_prog.GetID());
	sampler_pos = glGetUniformLocation(blinn_prog.GetID(), "gPosition");
	glUniform1i(sampler_pos, 0);
	sampler_pos = glGetUniformLocation(blinn_prog.GetID(), "gNormal");
	glUniform1i(sampler_pos, 1);
	sampler_pos = glGetUniformLocation(blinn_prog.GetID(), "gAlbedo");
	glUniform1i(sampler_pos, 2);
	sampler_pos = glGetUniformLocation(blinn_prog.GetID(), "ssao");
	glUniform1i(sampler_pos, 3);
}

void displayFunc()
{
	drawScene();
	glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	if(key == 27)
		glutLeaveMainLoop();
	if (key == 'w' || key == 'W') {
		camera->moveFront(0.02f);
		displayFunc();
	}
	if (key == 's' || key == 'S') {
		camera->moveFront(-0.02f);
		displayFunc();
	}
	if (key == 'a' || key == 'A') {
		camera->moveRight(-0.02f);
		displayFunc();
	}
	if (key == 'd' || key == 'D') {
		camera->moveRight(0.02f);
		displayFunc();
	}
	if (key == 'e' || key == 'E') {
		camera->moveWorldUp(0.02f);
		displayFunc();
	}
	if (key == 'q' || key == 'Q') {
		camera->moveWorldUp(-0.02f);
		displayFunc();
	}
}

void mouseFunc(int button, int state, int x, int y) {

}

void motionFunc(int x, int y)
{
	int deltaX = x - prevX;
	int deltaY = y - prevY;
	
	if(flightMode){
		camera->rotatePitch(-(float)deltaY / 5);
		camera->rotateYaw((float)deltaX / 5);
	}

	prevX = x;
	prevY = y;
	displayFunc();
}

void specialFunc(int key, int x, int y) {
	if (key == GLUT_KEY_F1) {
		flightMode = !flightMode;
	}
	if (key == GLUT_KEY_F2) {
		ssaoOn = !ssaoOn;
		updateTeapot();
		displayFunc();
	}

	switch (key) {
	case GLUT_KEY_LEFT:
		radius -= 0.2;
		updateTeapot();
		displayFunc();
		break;
	case GLUT_KEY_RIGHT:
		radius += 0.2;
		updateTeapot();
		displayFunc();
		break;
	case GLUT_KEY_UP:
		angle += 5;
		updateTeapot();
		displayFunc();
		break;
	case GLUT_KEY_DOWN:
		angle -= 5;
		updateTeapot();
		displayFunc();
		break;
	}
}

void idleFunc()
{

}

void setupFuncs() {
	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);
	glutIdleFunc(idleFunc);
	glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc(motionFunc);
}

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

int main(int argc, char** argv)
{
	//GLUT Init
	glutInit(&argc, argv);
	glutInitContextVersion(4, 5);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	scr_w = 1920;
	scr_h = 1080;

	glutInitWindowSize(scr_w, scr_h);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Final Project");
	
	setupFuncs();
	glClearColor(0, 1, 0, 1);

	// GLEW Init
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	
	glEnable(GL_DEPTH_TEST);

	// Setup Camera
	camera = new Camera(glm::vec3(-0.969084, 1.74454, -0.762535), glm::vec3(-50.6, 40.6, 0), 45.f, scr_w, scr_h);

	// 1 configure g-buffer
	{
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		// position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_w, scr_h, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gPosition, 0);
		// normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_w, scr_h, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, gNormal, 0);
		// color + specular color buffer
		glGenTextures(1, &gAlbedo);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scr_w, scr_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, gAlbedo, 0);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		// create and attach depth buffer (renderbuffer)
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_w, scr_h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	}

	// 2 SSAO Geo FB & Tex ---------------------------------------------
	glGenFramebuffers(1, &ssaoFrameBuffer);  glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFrameBuffer);
	// SSAO color buffer
	glGenTextures(1, &ssaoColorTex);
	glBindTexture(GL_TEXTURE_2D, ssaoColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, scr_w, scr_h, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ssaoColorTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;

	// 3 blur FB & Tex ---------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoTex);
	glBindTexture(GL_TEXTURE_2D, ssaoTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, scr_w, scr_h, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// generate random samples --------------------------------------
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		// scale samples
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale; 
		ssaoKernel.push_back(sample);
	}

	// generate a 4 x 4 noise texture
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// init porgs
	initShaders();
	
	// setup scene
	initScene();

	glutMainLoop();

	return 0;
}



//Screen constants