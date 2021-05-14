#include "Scene.h"
#include "Camera.h"
#include "FixedCam.h"
#include "MovableCam.h"
#include "RotationCam.h"
//#include "Main.cpp"

// Scene constructor, initilises OpenGL
// You should add further variables to need initilised.
Scene::Scene(Input *in)
{
	// Store pointer for input class
	input = in;

	//OpenGL settings
	glShadeModel(GL_SMOOTH);				// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// Cornflour Blue Background
	glClearDepth(1.0f);						// Depth Buffer Setup
	glClearStencil(0);						// Clear stencil buffer
	glEnable(GL_DEPTH_TEST);				// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);					// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// Really Nice Perspective Calculations
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Other OpenGL / render setting should be applied here.
	
	grass = SOIL_load_OGL_texture("gfx/grass.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	sand = SOIL_load_OGL_texture("gfx/sand.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	snow = SOIL_load_OGL_texture("gfx/snow_texture.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	//lights
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	speed = 100;

	//snow man
	InitialiseSnow();

	//cams
	current_camera = 0;
	cam.push_back(new MovableCam);
	cam[0]->SetLastMousePos(400, 300);
	cam.push_back(new FixedCam());
	cam.push_back(new RotationCam());
	number_cams = 3;
	changed_cam = false;

	
}

void Scene::handleInput(float dt)
{
	if (input->isKeyDown('c') && !changed_cam) {
		if (current_camera == number_cams-1) {
			current_camera = 0;
		}
		else {
			current_camera++;
		}
		changed_cam = true;
		cam[current_camera]->SetLastMousePos(input->getMouseX(),
											 input->getMouseY());
	}
	else if (!input->isKeyDown('c')){
		changed_cam = false;
	}
	cam[current_camera]->HandleInput(input, dt);

	if (input->isKeyDown('t')) {
		scorpion_x += speed*dt;
		if (scorpion_x > 0) {
			scorpion_x = 0;
		}
	}
	else if (input->isKeyDown('g')) {
		scorpion_x -= speed * dt;
		if (scorpion_x < -80) {
			scorpion_x = -80;
		}
	}
}

void Scene::update(float dt)
{
	// update scene related variables.
	cam[current_camera]->Update(dt);

	// Calculate FPS for output
	calculateFPS();

	rotation_light += speed * dt;

	ScorpionAnimation(dt);
}

void Scene::render() {
	Vector3 pos, front, up;
	int i;

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT 
			| GL_STENCIL_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	//setting lights
	//light 1
	GLfloat diffuse1[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse1);
	GLfloat light_pos1[4] = { 2.0f, 40.0f, 140.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos1);

	//light 2
	GLfloat diffuse2[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);
	GLfloat light_pos2[4] = { 17.0f, 20.0f, 7.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);
	GLfloat direction[4] = { 0.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);

	glPushMatrix();
	glTranslatef(-50.0, -50.0, -50.0);
	glRotatef(rotation_light, rotation_light, rotation_light, 0);
	glTranslatef(50.0, 50.0, 50.0);

	//light 3
	GLfloat diffuse3[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse3);
	GLfloat light_pos3[4] = { 50.0f, 50.0f, 50.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_POSITION, light_pos3);

	glPopMatrix();

	// Set the camera
	pos = cam[current_camera]->GetPos();
	front = cam[current_camera]->GetFront();
	up = cam[current_camera]->GetUp();
	gluLookAt(pos.x, pos.y, pos.z, 
		front.x+pos.x, front.y+pos.y, front.z+pos.z,
		up.x, up.y, up.z);

	//// Render geometry/scene here -------------------------------------

	//reflection settings
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glDisable(GL_DEPTH_TEST);
	DrawMirror();
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glPushMatrix();
	glTranslatef(200, 0, 0);
	glScalef(-1, 1, 1);
	RenderSceneObjects();
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	DrawMirror();
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	RenderSceneObjects();

	
	//// End render geometry --------------------------------------

	//// Render text, should be last object rendered.
	renderTextOutput();

	//// Swap buffers, after all objects are rendered.
	glutSwapBuffers();
	}


// Handles the resize of the window. If the window changes size the perspective matrix requires re-calculation to match new window size.
void Scene::resize(int w, int h) 
{
	width = w;
	height = h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;
	fov = 45.0f;
	nearPlane = 0.1f;
	farPlane = 500.0f;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(fov, ratio, nearPlane, farPlane);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);

	
}

// Calculates FPS
void Scene::calculateFPS()
{
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		sprintf_s(fps, "FPS: %4.2f", frame*1000.0 / ((LONG64)time - timebase));
		timebase = time;
		frame = 0;
	}
}

// Compiles standard output text including FPS and current mouse position.
void Scene::renderTextOutput()
{
	char buffer[128];
	float r = 1, g = 1, b = 1;
	
	displayText(-1.f, 0.90f, r, g, b, fps);

	sprintf_s(buffer, "Current Camera: %s", cam[current_camera]->GetName());
	displayText(-1.f, 0.85f, r, g, b, buffer);

	sprintf_s(buffer, "'c' changes current camera");
	displayText(-1.f, 0.80f, r, g, b, buffer);

	sprintf_s(buffer, "'t' and 'g' moves scorpion");
	displayText(-1.f, 0.75f, r, g, b, buffer);

	if (cam[current_camera]->GetName() == "movable") {
		sprintf_s(buffer, "'wasd' moves camera and mouse rotates");
		displayText(-1.f, 0.70f, r, g, b, buffer);
	}
	if (cam[current_camera]->GetName() == "rotation") {
		sprintf_s(buffer, "mouse rotates camera");
		displayText(-1.f, 0.70f, r, g, b, buffer);
	}
}

// Renders text to screen. Must be called last in render function (before swap buffers)
void Scene::displayText(float x, float y, float r, float g, float b, char* string) {
	// Get Lenth of string
	int j = strlen(string);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	// Swap to 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	glColor3f(r, g, b);
	glRasterPos2f(x, y);

	// Render text.
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, ((float)width/(float)height), nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void Scene::DrawSpider() {
	glPushMatrix();
	glEnable(GL_BLEND);
	glColor4f(1.0, 1.0, 1.0, 0.5);
	// Draw Spider body
	glPushMatrix();
	glTranslatef(50.0f, 5.0f, 10.0f);
	glutSolidSphere(5.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(45.0f, 5.0f, 10.0f);
	glutSolidSphere(3.0f, 20, 20);
	glPopMatrix();

	//draw "hands"
	glPushMatrix();
	glTranslatef(42.0f, 5.0f, 12.0f);
	glScaled(5.0, 1.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(42.0f, 5.0f, 8.0f);
	glScaled(5.0, 1.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	//lower part
	glPushMatrix();
	glTranslatef(40.0f, 3.0f, 12.0f);
	glScaled(1.0, 3.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(40.0f, 3.0f, 8.0f);
	glScaled(1.0, 3.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();


	//draw legs
	glPushMatrix();
	glTranslatef(46.0f, 5.0f, 10.0f);
	glScaled(1.0, 1.0, 20.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(49.0f, 5.0f, 10.0f);
	glScaled(1.0, 1.0, 20.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(52.0f, 5.0f, 10.0f);
	glScaled(1.0, 1.0, 20.0);
	glutSolidCube(1);
	glPopMatrix();

	//end of legs south
	glPushMatrix();
	glTranslatef(46.0f, 3.0f, 20.0f);
	glScaled(1.0, 5.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(49.0f, 3.0f, 20.0f);
	glScaled(1.0, 5.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(52.0f, 3.0f, 20.0f);
	glScaled(1.0, 5.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	//end of legs north
	glPushMatrix();
	glTranslatef(46.0f, 3.0f, 0.0f);
	glScaled(1.0, 5.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(49.0f, 3.0f, 0.0f);
	glScaled(1.0, 5.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(52.0f, 3.0f, 0.0f);
	glScaled(1.0, 5.0, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	// Draw Eyes
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(44.0f, 7.0f, 8.0f);
	glutSolidSphere(0.5f, 20, 20);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(44.0f, 7.0f, 12.0f);
	glutSolidSphere(0.5f, 20, 20);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(43.0f, 7.0f, 9.0f);
	glutSolidSphere(0.5f, 20, 20);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(43.0f, 7.0f, 11.0f);
	glutSolidSphere(0.5f, 20, 20);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	glPopMatrix();
}

void Scene::DrawScorpion() {
	GLdouble rotate = 20;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sand);
	glPushMatrix();
	glTranslatef(scorpion_x, 0, 0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	//body
	DrawScorpionBody();

	//draw legs
	DrawScorpionLegs();

	//arms
	DrawScorpionArms();

	//tail
	DrawScorpionTail();

	//eyes
	DrawScorpionEyes();

	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);

}

void Scene::CreateCube(GLfloat size) {
	static GLfloat n[6][3] =
	{
	  {-1.0, 0.0, 0.0},
	  {0.0, 1.0, 0.0},
	  {1.0, 0.0, 0.0},
	  {0.0, -1.0, 0.0},
	  {0.0, 0.0, 1.0},
	  {0.0, 0.0, -1.0}
	};
	static GLint faces[6][4] =
	{
	  {0, 1, 2, 3},
	  {3, 2, 6, 7},
	  {7, 6, 5, 4},
	  {4, 5, 1, 0},
	  {5, 6, 2, 1},
	  {7, 4, 0, 3}
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glTexCoord2f(0, 0);
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(0, 1);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(1, 1);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(1, 0);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

void Scene::CreateSnowMan(GLfloat rotation, GLfloat size, GLfloat posx, GLfloat posy,
						  GLfloat head, GLfloat eye, GLfloat nose,
						  Vector3 body_c, Vector3 head_c, Vector3 eyes_c)
{
	glPushMatrix();
	glRotatef(270, 1, 0,0);
	

	glTranslatef(posx, posy, 0); // translate to snowman's location
	glRotatef(rotation, 0, 0, 1);
	glScalef(size, size, size);

	// Draw body
	glPushMatrix();
	glColor3f(body_c.x, body_c.y, body_c.z);
	glTranslatef(0.0, 0.0, 0.75);
	DrawTexturedSphere(snow, 0.75, 20, 20);
	glPopMatrix();

	// Draw the head 
	glPushMatrix();
	glColor3f(head_c.x, head_c.y, head_c.z);
	glScaled(head, head, head);
	glTranslatef(0.0, 0.0, 1.75/head); // position head
	DrawTexturedSphere(snow, 0.25, 20, 20); // head sphere

	// Draw Eyes
	glColor3f(eyes_c.x, eyes_c.y, eyes_c.z); // eyes are black
	glPushMatrix();
	glScaled(eye, eye, eye);
	glTranslatef(0.0, -0.18/eye, 0.10/eye); // lift eyes to final position
	glPushMatrix();
	glTranslatef(-0.05 / eye, 0.0, 0.0);
	glutSolidSphere(0.05, 10, 10); // right eye
	glPopMatrix();
	glPushMatrix();
	glTranslatef(+0.05 / eye, 0.0, 0.0);
	glutSolidSphere(0.05, 10, 10); // left eye
	glPopMatrix();
	glPopMatrix();

	// Draw Nose (the nose is an orange cone)
	glColor3f(1.0, 0.5, 0.5); // nose is orange
	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0); // rotate to point along -y
	glutSolidCone(0.08, 0.5*nose, 10*nose, 2*nose); // draw cone
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
}

void Scene::InitialiseSnow() {
	int i;

	// positions of snow man
	Vector3 snow_pos[20] = {
		//third is angle
		Vector3(90, 50, 0),
		Vector3(70, 50, 0),
		Vector3(50, 50, 0),
		Vector3(30, 50, 0),
		Vector3(10, 50, 0),
		Vector3(-10, 50, 0),
		Vector3(-30, 50, 0),
		Vector3(-50, 50, 0),
		Vector3(-70, 50, 0),
		Vector3(-90, 50, 0),

		Vector3(90, -50, 180),
		Vector3(70, -50, 180),
		Vector3(50, -50, 180),
		Vector3(30, -50, 180),
		Vector3(10, -50, 180),
		Vector3(-10, -50, 180),
		Vector3(-30, -50, 180),
		Vector3(-50, -50, 180),
		Vector3(-70, -50, 180),
		Vector3(-90, -50, 180)
	};

	//set the random variables of the form of each snow man
	for (i = 0; i < number_snow; i++) {
		mans[i].rotation = snow_pos[i].z;
		mans[i].size = rand() % 10 + 1;
		mans[i].posx = snow_pos[i].x;
		mans[i].posy = snow_pos[i].y;
		mans[i].head = rand() % 5 + 1;
		mans[i].eye = rand() % 3 + 1;
		mans[i].nose = rand() % 3 + 1;
		mans[i].body_c = Vector3((float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX);
		mans[i].head_c = Vector3((float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX);
		mans[i].eyes_c = Vector3((float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX);
	}
}

void Scene::DrawMirror() {
	glBegin(GL_QUADS);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glVertex3f(100, 0, -50);
	glVertex3f(100, 50, -50);
	glVertex3f(100, 50, 50);
	glVertex3f(100, -50, 50);
	glPopMatrix();
	glEnd();
	
}

void Scene::DrawScorpionLeg(Vector3 pos) {
	
	
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScaled(1, 1, 20);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(pos.x, pos.y-2, pos.z+10);
	glScaled(1, 5, 1);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(pos.x, pos.y-2, pos.z-10);
	glScaled(1, 5, 1);
	CreateCube(1);
	glPopMatrix();
}

void Scene::DrawScorpionEye(Vector3 pos, float scale) {

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(1, scorpion_eye_scale, 1);
	glutSolidSphere(0.5f, 20, 20);
	glPopMatrix();
}

void Scene::DrawScorpionTail() {

	glPushMatrix();

	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glTranslatef(5.0f, 6.0f, 10.0f);
	glScaled(12.0, 3.0, 5.0);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 6.0f, 10.0f);
	glScaled(11.0, 2.0, 4.0);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5.0f, 6.0f, 10.0f);
	glScaled(10.0, 1.0, 3.0);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-9.0f, 8.0f, 10.0f);
	glScaled(2.0, 3.0, 2.0);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-9.0f, 10.0f, 10.0f);
	glScaled(1.0, 3.0, 1.0);
	CreateCube(1);
	glPopMatrix();

	glPopMatrix();
}

void Scene::DrawScorpionArms() {

	//"arms"
	glPushMatrix();
	glTranslatef(30.0f, 6.0f, 10.0f);
	glScaled(1.0, 1.0, 20.0);
	CreateCube(1);
	glPopMatrix();

	//"hand" south
	DrawScorpionHand(Vector3(30, 6, 20));

	//"hand" north
	DrawScorpionHand(Vector3(30, 6, 1));
}

void Scene::DrawScorpionEyes() {
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	
	DrawScorpionEye(Vector3(32, 7, 8), scorpion_eye_scale);

	DrawScorpionEye(Vector3(32, 7, 12), scorpion_eye_scale);

	DrawScorpionEye(Vector3(32, 5, 11), scorpion_eye_scale);

	DrawScorpionEye(Vector3(32, 5, 9), scorpion_eye_scale);
	glPopMatrix();
}

void Scene::DrawScorpionHand(Vector3 pos) {

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScaled(3.0, 2.0, 3.0);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(pos.x+3, pos.y, pos.z+1);
	glScaled(3.0, 1.0, 1.0);
	CreateCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(pos.x+3, pos.y, pos.z-1);
	glScaled(3.0, 1.0, 1.0);
	CreateCube(1);
	glPopMatrix();
}

void Scene::DrawScorpionLegs() {
	DrawScorpionLeg(Vector3(10, 5, 10));

	DrawScorpionLeg(Vector3(15, 5, 10));

	DrawScorpionLeg(Vector3(20, 5, 10));

	DrawScorpionLeg(Vector3(25, 5, 10));
}

void Scene::DrawScorpionBody() {
	glPushMatrix();
	glTranslatef(20.0f, 5.0f, 10.0f);
	glScaled(4.0, 1.0, 1.0);
	CreateCube(6);
	glPopMatrix();
}

void Scene::ScorpionAnimation(float dt) {
	int max_angle = 10, rotation_speed = 50, i;
	float blink_time = 5, blink_speed = 10;
	//scorpion rotating
	if (!rising) {
		scorpion_angle += rotation_speed*dt;
		if (scorpion_angle >= max_angle) {
			rising = false;
		}
	}
	else {
		scorpion_angle -= rotation_speed*dt;
		if (scorpion_angle <= -max_angle) {
			rising = true;
		}
	}

	//eye blinking
	
	if (!blinking) {
		blink_timer += dt;
		if (blink_timer >= blink_time) {
			blinking = true;
			blink_timer = 0;
		}
	}
	else {
		if (!increasing_eye) {
			scorpion_eye_scale -= blink_speed * dt;
			if (scorpion_eye_scale <= 0) {
				scorpion_eye_scale = 0;
				increasing_eye = true;
			}
		}
		else {
			scorpion_eye_scale += blink_speed * dt;
			if (scorpion_eye_scale >= 1) {
				scorpion_eye_scale = 1;
				increasing_eye = false;
				blinking = false;
			}
		}

	}
}

void Scene::RenderSceneObjects() {
	int i;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grass);

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-100.0f, 0.0f, -100.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-100.0f, 0.0f, 100.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 1);
	glVertex3f(100.0f, 0.0f, 100.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 0);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, NULL);
	glDisable(GL_TEXTURE_2D);

	DrawScorpion();

	glPushMatrix();

	// Turn on wireframe mode
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	glTranslatef(40, 50, 0);
	glRotatef(270, 0, 0, 1);
	glTranslatef(0, 0, 10);
	glRotatef(scorpion_angle, 1, 0, 0);
	glTranslatef(0, 0, -10);
	
	DrawScorpion();

	// Turn off wireframe mode
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glPopMatrix();

	//create the snow man
	for (i = 0; i < number_snow; i++) {
		CreateSnowMan(mans[i].rotation, mans[i].size, mans[i].posx, mans[i].posy,
			mans[i].head, mans[i].eye, mans[i].nose,
			mans[i].body_c, mans[i].head_c, mans[i].eyes_c);

	}

	//create the spider
	DrawSpider();
}

void Scene::DrawTexturedSphere(GLuint texture_id,
								float radius, float slices,
								float stacks) {
	GLUquadricObj *qobject = gluNewQuadric();
	
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);

	gluQuadricDrawStyle(qobject, GLU_FILL);

	glBindTexture(GL_TEXTURE_2D, texture_id);

	gluQuadricTexture(qobject, GL_TRUE);

	gluQuadricNormals(qobject, GLU_SMOOTH);

	gluSphere(qobject, radius, slices, stacks);

	glPopMatrix();
}
