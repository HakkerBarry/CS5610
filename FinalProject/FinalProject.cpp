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

using namespace cy;

bool flightMode, gBufferMode;
int prevX, prevY;
ObjDrawer* objDrawer, *plane, *teapot2;
Camera* camera;

GLSLProgram SSAO_Geo_prog;
GLSLProgram simple_prog;

//Gbuffer
GLuint gBuffer;
GLuint ssaoFBO, ssaoBlurFBO;

int scr_w, scr_h;

void drawScene() {
	if (gBufferMode) {
		objDrawer->setProg(SSAO_Geo_prog.GetID());
		teapot2->setProg(SSAO_Geo_prog.GetID());
		plane->setProg(SSAO_Geo_prog.GetID());
	}
	else {
		objDrawer->setProg(simple_prog.GetID());
		teapot2->setProg(simple_prog.GetID());
		plane->setProg(simple_prog.GetID());
	}
	objDrawer->draw(camera->getView(), camera->getProj());
	teapot2->draw(camera->getView(), camera->getProj());
	plane->draw(camera->getView(), camera->getProj());
}

void initScene() {
	// Setup model-----------------------------------------
	objDrawer->setProg(simple_prog.GetID());
	objDrawer->setAttrib();
	objDrawer->setPosition(glm::vec3(0, 0, 0));
	objDrawer->setScale(glm::vec3(.05f, .05f, .05f));
	objDrawer->setRotation(glm::vec3(-90, 0, -45));

	teapot2->setProg(simple_prog.GetID());
	teapot2->setAttrib();
	teapot2->setPosition(glm::vec3(1, 0, 0));
	teapot2->setScale(glm::vec3(.05f, .05f, .05f));

	plane->setProg(simple_prog.GetID());
	plane->setAttrib();
	plane->setPosition(glm::vec3(0, 0, 0));
	plane->setScale(glm::vec3(1.f, 1.f, 1.f));
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
}

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		gBufferMode = !gBufferMode;
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

	scr_w = 1920;
	scr_h = 1080;

	glutInitWindowSize(scr_w, scr_h);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Final");
	
	setupFuncs();
	glClearColor(0, 0, 0, 0);

	// GLEW Init
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	glutInitContextVersion(4, 5);
	glEnable(GL_DEPTH_TEST);

	// Setup Camera
	camera = new Camera(glm::vec3(-0.969084, 1.74454, -0.762535), glm::vec3(-50.6, 40.6, 0), 45.f, scr_w, scr_h);

	// Load Mesh
	objDrawer = new ObjDrawer("res/teapot.obj", false);
	teapot2 = new ObjDrawer("res/teapot.obj", false);
	plane = new ObjDrawer("res/plane.obj", false);

	// configure g-buffer framebuffer
	// ------------------------------
	
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedo;
	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_w, scr_h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_w, scr_h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scr_w, scr_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_w, scr_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// also create framebuffer to hold SSAO processing stage 
	// -----------------------------------------------------
	glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
	// SSAO color buffer
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, scr_w, scr_h, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;
	// and blur stage
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, scr_w, scr_h, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// generate sample kernel
	// ----------------------
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	// generate noise texture
	// ----------------------
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}

	unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
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