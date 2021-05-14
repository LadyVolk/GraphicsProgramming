// Scene class. Configures a basic 3D scene.
// Interfaces with the Input class to handle user input
// Calculates and outputs Frames Per Second (FPS) rendered.
// Important functions are the constructor (initialising the scene), 
// update (for process user input and updating scene objects) and render (renders scene).
#ifndef _SCENE_H
#define _SCENE_H

// Include GLUT, openGL, input.
#include "glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Input.h"
#include <stdio.h>
// Further includes should go here:
#include "SOIL.h"
#include <vector>
#include "Camera.h"
#include <iostream>

//use for storing information for each snowman
struct SnowManData {
	GLfloat size, posx, posy, head, eye, nose;
	Vector3 body_c, head_c, eyes_c;
	GLfloat rotation;
};

class Scene{

public:
	//variables
	int current_camera;
	bool changed_cam;
	int number_cams;
	std::vector <Camera*> cam;
	

	Scene(Input *in);
	// Main render function
	void render();
	// Handle input function that receives delta time from parent.
	void handleInput(float dt);
	// Update function receives delta time from parent (used for frame independent updating).
	void update(float dt);
	// Resizes the OpenGL output based on new window size.
	void resize(int w, int h);

	//added functions by me
	void renderScene();
	void DrawSpider();
	void DrawScorpion();
	void InitialiseSnow();
	void CreateCube(GLfloat size);
	void CreateSnowMan(GLfloat rotation, GLfloat size, GLfloat posx, GLfloat posy,
		GLfloat head, GLfloat eye, GLfloat nose,
		Vector3 body_c, Vector3 head_c, Vector3 eyes_c);
	void DrawMirror();
	void DrawScorpionLeg(Vector3 pos);
	void DrawScorpionEye(Vector3 pos, float scale);
	void DrawScorpionTail();
	void DrawScorpionArms();
	void DrawScorpionEyes();
	void DrawScorpionHand(Vector3 pos);
	void DrawScorpionLegs();
	void DrawScorpionBody();
	void ScorpionAnimation(float dt);
	void RenderSceneObjects();
	void DrawTexturedSphere(GLuint texture_id, float radius, float slices,
		float stacks);
	void drawSnowMan();

protected:

	struct SnowManData mans[20];

	// Renders text (x, y positions, RGB colour of text, string of text to be rendered)
	void displayText(float x, float y, float r, float g, float b, char* string);
	void processSpecialKeys(int key, int xx, int yy);

	// A function to collate all text output in a single location
	void renderTextOutput();
	void calculateFPS();
	
	// For access to user input.
	Input* input;
		
	// For Window and frustum calculation.
	int width, height;
	float fov, nearPlane, farPlane;

	// For FPS counter and mouse coordinate output.
	int frame = 0, time, timebase = 0;
	char fps[40];
	char mouseText[40];
	int number_snow = 20;

	//other variables
	float scorpion_x = 0;
	float scorpion_speed = 20;
	float speed;
	float rotation_light;
	float scorpion_eye_scale = 1;
	float scorpion_angle = 0;
	bool rising = false;
	bool increasing_eye = false;
	bool blinking = false;
	float blink_timer = 0;

	//Render Texture 
	const int noLights = 1;
	
	//textures
	GLuint grass;

	GLuint sand;

	GLuint snow;
};

#endif