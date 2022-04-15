/*
Zixuan Zhang A2 for CS5610 at the UofU
*/

#ifndef CCRender
#define CCRender
#include "render.h"
#endif
#ifndef GL_H
#define GL_H
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyGL.h"
#include "lodepng.h"
#include <iostream>
#include <fstream>
#include <vector>

#endif
#include <random>
#include <iostream>

CC::Scene* scene = nullptr;
CC::Camera* camera = nullptr;
std::vector<CC::WorldObject*>* worldObjects = nullptr;
CC::TextureCube* environmentMap = nullptr;
int w = 1024;
int h = 1024;
bool leftButtonHold = false;
bool rightButtonHold = false;
int _x = 0;
int _y = 0;

GLuint gBuffer, ssaoFBO, ssaoBlurFBO;

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void displayFunc() {
    if (!scene) {
        std::cout << "Display Error: scene not found";
        exit(1);
    }
    //Clear the viewport
    glClearColor(0.5, 0.5, 0.5, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    scene->Draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    scene->Draw();
    glutSwapBuffers();

}

void keyboardFunc(unsigned char key, int x, int y) {
    //Handle keyboard input here
    CC::vec3 pos;
    switch (key) {
    case 27: //ESC
        glutLeaveMainLoop();
        break;
    case 'w':
        camera->cameraDirectionMove(0.05);
        pos = camera->getPosition();
        std::cout << pos.x << " " << pos.y << " " << pos.z << "\n";
        break;
    case 's':
        camera->cameraDirectionMove(-0.05);
        pos = camera->getPosition();
        std::cout << pos.x << " " << pos.y << " " << pos.z << "\n";
        break;
    case 'd':
        camera->cameraHorizontalMove(0.05);
        break;
    case 'a':
        camera->cameraHorizontalMove(-0.05);
        break;
    }
}

void mouseFunc(int button, int state, int x, int y) {

    switch (button) {
    case 2://right
        if (!leftButtonHold) {
            rightButtonHold = (state == 0);
        }
        break;
    case 0://left
        if (!rightButtonHold) {
            leftButtonHold = (state == 0);
        }
        break;
    }

}

void mouseMotionFunc(int x, int y) {

    if (_x != x) {
        if (rightButtonHold) {
            float delta = _x > x ? -0.2 : 0.2;


        }
        else if (leftButtonHold) {
            float deltaY = _x > x ? -0.2 : 0.2;
            //rotY += deltaY;
            camera->horizontalRotate(deltaY);
        }
        _x = x;
    }
    else if (_y != y) {
        if (leftButtonHold) {
            float deltaX = _y > y ? 0.05 : -0.05;
            //rotX += deltaX;
            camera->verticalRotate(deltaX);
        }
        _y = y;
    }

}

void idleFunc() {
    //Handle animations here
    //Tell GLUT to redraw
    glutPostRedisplay();
}

void createWindow() {
    //Create a window
    glutInitWindowSize(w, h);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("CS5610");
}

void registerCallback() {
    //register callback function
    glutDisplayFunc(displayFunc);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseMotionFunc);
    glutIdleFunc(idleFunc);
}

void initializeScene() {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedo;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_FLOAT, NULL);
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


    // global varible
    camera = new CC::Camera();
    CC::SimpleShader* meshShader = new CC::SimpleShader();
    //CC::MeshShader* meshShader = new CC::MeshShader();
    worldObjects = new std::vector<CC::WorldObject*>();
    CC::Material* defaultMaterial = new CC::Material(meshShader, 500, CC::vec3(0.95, 0.95, 0.95), CC::vec3(0.5, 0.5, 0.5));
    CC::Light* light = new CC::Light(CC::vec3(0.2, 0.2, 0.2), CC::vec3(10, 10, 10), CC::vec3(0.9, 0.9, 0.9));
    // obj 1
    CC::PolygonalMesh* teapot = new CC::PolygonalMesh("res/teapot.obj");
    CC::RenderObject* teapot_renderObject = new CC::RenderObject(teapot, defaultMaterial);
    CC::WorldObject* Obj1 = new CC::WorldObject(teapot_renderObject, light, camera, CC::vec3(0, 0, 0), CC::vec3(1, 1, 1), 0);
    worldObjects->push_back(Obj1);
    // obj 2
    CC::PolygonalMesh* plate = new CC::PolygonalMesh("res/plate.obj");
    CC::RenderObject* plate_renderObject = new CC::RenderObject(plate, defaultMaterial);
    CC::WorldObject* Obj2 = new CC::WorldObject(plate_renderObject, light, camera, CC::vec3(0, -0.5, 0), CC::vec3(4, 4, 4), 0);
    worldObjects->push_back(Obj2);
    // obj 3
    CC::WorldObject* Obj3 = new CC::WorldObject(teapot_renderObject, light, camera, CC::vec3(1, 0, 0.5), CC::vec3(0.5, 0.5, 0.5), 145);
    worldObjects->push_back(Obj3);
    CC::WorldObject* Obj4 = new CC::WorldObject(teapot_renderObject, light, camera, CC::vec3(-1, 0, -1), CC::vec3(0.5, 0.5, 0.5), 60);
    worldObjects->push_back(Obj4);

    scene = new CC::Scene(worldObjects, environmentMap, camera);
}

int main(int argc, char** argv) {
    //GLUT initialization
    glutInit(&argc, argv);
    createWindow();
    registerCallback();
    glewInit();//Init after create window
    std::cout << "Start Initialize Scene";
    initializeScene();
    std::cout << "Start Main Loop";
    //call main loop
    glutMainLoop();
    return 0;
}