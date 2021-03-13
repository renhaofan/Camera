#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <windows.h>
#include <fstream>
#include <strstream>
using std::string;
using std::cout;
using std::endl;
using std::ifstream;

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>


//#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <Eigen/SparseLU>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include "GLCamera.hpp"
// constexpr auto DEG2RAD = (3.1415926535f / 180.0f);
#define DEG2RAD (3.1415926535f / 180.0f)
#define RAD2DEG (180.0f / 3.1415926535f)

typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;



// reference: http://www.songho.ca/opengl/gl_camera.html#lookat


// s0 centroid points 333.661 169.086 - 21.2678
std::string s0_path = "s0.obj";
std::string s1_path = "s1.obj";
std::string t0_path = "t0.obj";
std::string t1_path = "t1.obj";

MyMesh s0;
MyMesh s1;
MyMesh t0;
MyMesh t1;

// compute frames per second
// Note that this shouldn’t be considered a true benchmark, just a guide value.
int frame = 0;
size_t current_time, time_stamp;
char fps_chars[50];

// keyboard concerned
GLboolean normal_keys_status[256] = { false };
GLboolean special_keys_status[256] = { false };
float normal_key_speed = 1.f;
float special_key_speed = 1.f;

// Pop up menu identifiers
int main_menu, fill_menu, axes_menu;
bool show_axes = true;



// width and height of the window
int window_height = 800;
int window_width = 800;
int screen_height = glutGet(GLUT_SCREEN_HEIGHT);
int screen_width = glutGet(GLUT_SCREEN_WIDTH);


GLCamera camera;
// camera concerned
float camera_position[3] = {0.f, 0.f, -10.f};
float camera_angle[3] = { 0.f, 0.f, 0.f};
float mouse_speed = 0.001f;
float mouse_delta_move[3] = { 0.f };
int x_origin = -1;

// teapot
float teapot_rotate = 0.f;
bool teapot_is_rotate_bool = false;
// render way LINE/FILL
int render_way = 1;

// variables to hold window identifiers
int main_window;
int sub_window[3];
// distance from camera position to top-view camera position
float top_view_distance = 5.f;
float observation_view_distance = 10.f;
// border between subwindow
int border = 6;

void init();


void setProjection(int w1, int h1) {
	float ratio;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	ratio = 1.0f * w1 / h1;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w1, h1);

	// Set the clipping volume
	gluPerspective(45, ratio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void changeSize(int w, int h) {
	window_height = h;
	window_width = w;

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	
	// set subwindow 1 as the active window
	glutSetWindow(sub_window[0]);
	// resize and reposition the sub window
	glutPositionWindow(border, border);
	glutReshapeWindow(w - 2 * border, h / 2 - border * 3 / 2);
	setProjection(w - 2 * border, h / 2 - border * 3 / 2);

	// set subwindow 2 as the active window
	glutSetWindow(sub_window[1]);
	// resize and reposition the sub window
	glutPositionWindow(border, (h + border) / 2);
	glutReshapeWindow(w / 2 - border * 3 / 2, h / 2 - border * 3 / 2);
	setProjection(w / 2 - border * 3 / 2, h / 2 - border * 3 / 2);

	// set subwindow 3 as the active window
	glutSetWindow(sub_window[2]);
	// resize and reposition the sub window
	glutPositionWindow((w + border) / 2, (h + border) / 2);
	glutReshapeWindow(w / 2 - border * 3 / 2, h / 2 - border * 3 / 2);
	setProjection(w / 2 - border * 3 / 2, h / 2 - border * 3 / 2);
}


void renderBitmapString(float x, float y, float z, void *font, char *string) {
	// example: renderBitmapString(1.0f, 0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"X");
	char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
void plotWorldAxes() {
	float half_length = 2.f;
	// draw axess
	glPushMatrix();
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(-half_length, 0.0f, 0.0f);
	glVertex3f(half_length, 0.0f, 0.0f);

	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.f, -half_length, 0.0f);
	glVertex3f(0.f, half_length, 0.0f);

	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, -half_length);
	glVertex3f(0.f, 0.f, half_length);
	glEnd();
	glLineWidth(1.0f);
	glPopMatrix();
	// draw cone
	glColor3f(1.f, 1.f, 1.f);
	glPushMatrix();
	//glColor3f(1.f, 0.f, 0.f);
	glTranslatef(half_length, 0, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidCone(0.1, 0.3, 16, 16);
	glPopMatrix();

	glPushMatrix();
	//glColor3f(0.f, 1.f, 0.f);
	glTranslatef(0, half_length, 0);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(0.1, 0.3, 16, 16);
	glPopMatrix();

	glPushMatrix();
	//glColor3f(0.f, 0.f, 1.f);
	glTranslatef(0, 0, half_length);
	glutSolidCone(0.1, 0.3, 16, 16);
	glPopMatrix();

	// draw sephere
	glPushMatrix();
	glTranslatef(-half_length, 0, 0);
	glutSolidSphere(0.03, 6, 6);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -half_length, 0);
	glutSolidSphere(0.03, 6, 6);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -half_length);
	glutSolidSphere(0.03, 6, 6);
	glPopMatrix();

	glPushMatrix();
	renderBitmapString(half_length + 0.3f, 0.f, 0.f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"X");
	renderBitmapString(0.f, half_length + 0.3f, 0.f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"Y");
	renderBitmapString(0.f, 0.f, half_length + 0.3f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"Z");
	glPopMatrix();
}
void plotReferenceGrid(float start = 20.0f, float gridSize = 1.0f) {
	glPushMatrix();
	glColor3f(0.5f, 0.5, 0.5); // gray color
	for (float i = -start; i <= start; i++) {
		glBegin(GL_LINES);
		glVertex3f(i*gridSize, -1.f, -start);
		glVertex3f(i*gridSize, -1.f, start);
		glVertex3f(-start, -1.f, i*gridSize);
		glVertex3f(start, -1.f, i*gridSize);
		glEnd();
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}


void plotTeaTable() {
	glColor3f(1.f, 1.f, 1.f);
	// draw teapot
	glPushMatrix();
	glTranslatef(0, 5, 0);
	glRotatef(teapot_rotate, 0, 1, 0);
	glutSolidTeapot(1.f);
	glPopMatrix();

	// draw table
	glPushMatrix();
	glScalef(6, 1, 6);
	glTranslatef(0, 2.5, 0);
	glutSolidCube(1.f);
	glPopMatrix();

	// draw legs
	glPushMatrix();
	glTranslatef(1.5, 1, 1.5);
	glScalef(1, 2, 1);
	glutSolidCube(1.f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 1, 1.5);
	glScalef(1, 2, 1);
	glutSolidCube(1.f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, 1, -1.5);
	glScalef(1, 2, 1);
	glutSolidCube(1.f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 1, -1.5);
	glScalef(1, 2, 1);
	glutSolidCube(1.f);
	glPopMatrix();
}
void plotMesh(MyMesh* m) {
	glPushMatrix();

	glColor3f(1.f, 1.f, 1.f);
	for (MyMesh::FaceIter f_it = (*m).faces_begin(); f_it != (*m).faces_end(); ++f_it) {
		int index = 0;
		glBegin(GL_LINE_LOOP);
		// fv_it minus -1 automeshically, make sure vertex index from 0
		for (MyMesh::FaceVertexIter fv_it = (*m).fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
			auto p = (*m).point(*fv_it);
			float * point = p.data();
			glVertex3fv(point);
		}
		glEnd();
	}
	glPopMatrix();
}
void plotCamera(float* position) {
	// camera concerned
	float height = 1.f; // y
	float length = 2.f; //  x
	float width = 0.5f; // z
	float button_height = width / 4;
	float radius = height - 0.5;
	radius /= 2;
	std::vector<float> s;
	// axis concerned
	float axis_length = length;


	// draw camera body
	glPushMatrix();
	glColor3f(1.f, 1.f, 1.f);
	glTranslatef(position[0], position[1], position[2]);
	glScalef(length, height, width);
	glutSolidCube(1.f);
	glPopMatrix();

	// draw lens
	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glBegin(GL_QUAD_STRIP);
	for (size_t i = 0; i <= 360; i += 15) {
		float p = i * 3.14 / 180;
		float sinp = std::sin(p);
		float cosp = std::cos(p);
		glColor3f(sin(p), cos(p), 1.0f);
		glVertex3f(sinp * radius, cosp * radius, width / 2);
		glVertex3f(sinp * radius, cosp * radius, width);
		s.push_back(sinp);
		s.push_back(cosp);
	}
	glEnd();
	glPopMatrix();

	// draw button
	radius = width / 4;
	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glBegin(GL_QUAD_STRIP);
	size_t tmp = s.size();
	tmp = tmp / 2;
	for (size_t i = 0; i < tmp; ++i) {
		float p = i * 3.14 / 180;
		float sinp = s[i * 2];
		float cosp = s[i * 2 + 1];
		glColor3f(sinp, cosp, 1.0f);
		glVertex3f(sinp * radius - length/2+radius+0.1f, height/2, cosp * radius);
		glVertex3f(sinp * radius - length/2+radius+0.1f, height/2+button_height, cosp * radius);
	}
	glEnd();
	glPopMatrix();

	// draw axes
	glPushMatrix();
	glLineWidth(5.0f);
	glTranslatef(position[0], position[1], position[2]);
	glBegin(GL_LINES);
	glColor3f(1.f, 0.f, 0.f); // u
	glVertex3f(0.f, 0.0f, 0.0f);
	glVertex3f(-axis_length, 0.0f, 0.0f);
	glColor3f(0.f, 1.f, 0.f); // v
	glVertex3f(0.f, 0.0f, 0.0f);
	glVertex3f(0.f, axis_length, 0.0f);
	glColor3f(0.f, 0.f, 1.f); // -w
	glVertex3f(0.f, 0.0f, 0.0f);
	glVertex3f(0.f, 0.f, axis_length);
	glEnd();
	glLineWidth(1.0f);
	glPopMatrix();
}

void myplotCone() {
	// reference :https://www.cnblogs.com/MakeView660/p/10436685.html
	glPushMatrix();
	glBegin(GL_QUAD_STRIP);//连续填充四边形串
	int i = 0;
	for (i = 0; i <= 360; i += 15)
	{
		float p = i * 3.14 / 180;
		glColor3f(sin(p), cos(p), 1.0f);
		glVertex3f(0, 0, 1.0f);
		glVertex3f(sin(p), cos(p), 0.0f);
	}
	glEnd();
	//bottom circle
	//glColor3f(0, 1, 1);
	//glBegin(GL_TRIANGLE_FAN);           //扇形连续填充三角形串
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//for (i = 0; i <= 360; i += 15)
	//{
	//	float p = i * 3.14 / 180;
	//	glColor3f(sin(p), cos(p), tan(p));
	//	glVertex3f(sin(p), cos(p), 0.0f);
	//}
	//glEnd();
	glPopMatrix();
}

void normalKeyStatus() {
	if (normal_keys_status['w'] || normal_keys_status['W']) {
		camera.ShiftForward(0.1 * normal_key_speed);
	}
	if (normal_keys_status['s'] || normal_keys_status['S']) {
		camera.ShiftBackward(0.1 * normal_key_speed);
	}
	if (normal_keys_status['a'] || normal_keys_status['A']) {
		camera.ShiftLeft(0.1 * normal_key_speed);
	}
	if (normal_keys_status['d'] || normal_keys_status['D']) {
		camera.ShiftLeft(-0.1 * normal_key_speed);
	}
	if (normal_keys_status['q'] || normal_keys_status['Q']) {
		camera.ShiftUp(0.1 * normal_key_speed);
	}
	if (normal_keys_status['e'] || normal_keys_status['E']) {
		camera.ShiftUp(-0.1 * normal_key_speed);
	}
}
void specialKeyStatus() {
	if (special_keys_status[GLUT_KEY_LEFT]) {
		camera.YawV(-1.f * DEG2RAD * special_key_speed);
	}
	if (special_keys_status[GLUT_KEY_RIGHT]) {
		camera.YawV(1.f * DEG2RAD * special_key_speed);
	}
	if (special_keys_status[GLUT_KEY_UP]) {
		camera.PitchU(-1.f * DEG2RAD * special_key_speed);
	}
	if (special_keys_status[GLUT_KEY_DOWN]) {
		camera.PitchU(1.f * DEG2RAD * special_key_speed);
	}
}
void mouseMoveStatus() {
	if (x_origin != -1) {
		camera.YawV(mouse_delta_move[0]);
	}
}

void setOrthoProjection() {
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();

	// reset matrix
	glLoadIdentity();

	// set a 2D orthographic projection
	gluOrtho2D(0, window_width, window_height, 0);

	// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}
void restorePerspProjection() {
	glMatrixMode(GL_PROJECTION);
	// restore previous projection matrix
	glPopMatrix();

	// get back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}

void plotObject() {
	// plot reference grid
	plotReferenceGrid(20.f);
	// plot world coordinate axis
	if (show_axes) plotWorldAxes();

	float* camera_e = camera.GetCameraE().data();
	camera_position[0] = *camera_e;
	camera_position[1] = *(camera_e + 1);
	camera_position[2] = *(camera_e + 2);
	plotCamera(camera_position);
	if (teapot_is_rotate_bool) teapot_rotate += 1.f;

	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 }; // 定义颜色
	GLfloat light_pos[] = { 5,5,5,1 };  //定义光源位置
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos); //设置第0号光源的光照位置
	glLightfv(GL_LIGHT0, GL_AMBIENT, white); //设置第0号光源多次反射后的光照颜色（环境光颜色）


	glEnable(GL_LIGHTING); //开启光照模式
	glEnable(GL_LIGHT0); //开启第0号光源
	plotTeaTable();
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	// plot mesh
	plotMesh(&s0);
}

// Display func for main window
void renderScene() {
	glutSetWindow(main_window);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void renderSw0Scene() {
	glutSetWindow(sub_window[0]);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (render_way) {
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}
	

	normalKeyStatus();
	specialKeyStatus();
	mouseMoveStatus();


	// Reset transformations
	glLoadIdentity();
	Eigen::Matrix4f matrixModelView = camera.GetViewMatrix();
	float m[16];
	m[0] = matrixModelView(0, 0); m[4] = matrixModelView(0, 1);  m[8] = matrixModelView(0, 2);  m[12] = matrixModelView(0, 3);
	m[1] = matrixModelView(1, 0); m[5] = matrixModelView(1, 1);  m[9] = matrixModelView(1, 2);  m[13] = matrixModelView(1, 3);
	m[2] = matrixModelView(2, 0); m[6] = matrixModelView(2, 1);  m[10] = matrixModelView(2, 2); m[14] = matrixModelView(2, 3);
	m[3] = matrixModelView(3, 0); m[7] = matrixModelView(3, 1);  m[11] = matrixModelView(3, 2);  m[15] = matrixModelView(3, 3);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m);




	//float mat[16];  //按照列存储，所以按照行打出来
	//glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	//for (int index = 0; index < 4; index++) {
	//	for (int i = 0; i < 4; ++i) {
	//		printf("%f ", mat[index + i * 4]);
	//	}
	//	cout << endl;
	//}
	//cout << endl;

	// display fps in the window
	frame++;
	current_time = glutGet(GLUT_ELAPSED_TIME); // milisecond
	if (current_time - time_stamp > 1000) {
		sprintf(fps_chars, "FPS:%4.2f", frame * 1000.f / (current_time - time_stamp));
		time_stamp = current_time;
		frame = 0;
	}

	// display fps
	setOrthoProjection();
	void *font = GLUT_BITMAP_8_BY_13;
	glPushMatrix();
	glLoadIdentity();

	renderBitmapString(5, 40, 0, font, fps_chars);
	renderBitmapString(30, 70, 0, font, (char *)"F1 - Full Screen");
	renderBitmapString(30, 100, 0, font, (char *)"F2 - Window Mode");
	renderBitmapString(30, 130, 0, font, (char *)"Esc - Quit");

	glPopMatrix();
	restorePerspProjection();

	plotObject();

	glutSwapBuffers();
}

void renderSw1Scene() {
	glutSetWindow(sub_window[1]);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (render_way) {
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}
	plotObject();
	normalKeyStatus();
	specialKeyStatus();
	mouseMoveStatus();

	glLoadIdentity();
	Eigen::Vector3f top_view[4];
	top_view[0] = camera.GetCameraU();
	top_view[1] = camera.GetCameraV();
	top_view[2] = camera.GetCameraW();
	top_view[3] = camera.GetCameraE();

	gluLookAt(top_view[3].x(), top_view_distance * ((top_view[1]+top_view[3]).y()), top_view[3].z(),
		      top_view[3].x(), top_view[3].y(), top_view[3].z(), 
			  -top_view[2].x(), -top_view[2].y(), -top_view[2].z());


	glutSwapBuffers();
}

void renderSw2Scene() {
	glutSetWindow(sub_window[2]);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (render_way) {
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}
	plotObject();
	normalKeyStatus();
	specialKeyStatus();
	mouseMoveStatus();

	glLoadIdentity();
	Eigen::Vector3f obser_view[4];
	obser_view[0] = camera.GetCameraU();
	obser_view[1] = camera.GetCameraV();
	obser_view[2] = camera.GetCameraW();
	obser_view[3] = camera.GetCameraE();

	
	gluLookAt(observation_view_distance + obser_view[3].x(), observation_view_distance + obser_view[3].y(), observation_view_distance + obser_view[3].z(),
			  obser_view[3].x(), obser_view[3].y(), obser_view[3].z(),
			  obser_view[1].x(), obser_view[1].y(), obser_view[1].z());

	glutSwapBuffers();
}
// Global idle func
void renderAllScene() {
	normalKeyStatus();
	specialKeyStatus();
	mouseMoveStatus();

	renderSw0Scene();
	renderSw1Scene();
	renderSw2Scene();
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch (key) {
	case 27://key ESC
	/*	glutDestroyMenu(axes_menu);
		glutDestroyMenu(fill_menu);
		glutDestroyMenu(main_menu);*/
		glutDestroyWindow(sub_window[0]);
		glutDestroyWindow(sub_window[1]);
		glutDestroyWindow(sub_window[2]);
		glutDestroyWindow(main_window);
		if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE) != 0)
			glutLeaveGameMode();
		exit(0); 
		break; 
	case 'w': normal_keys_status['w'] = true; break;
	case 'W': normal_keys_status['W'] = true; break;
	
	case 's': normal_keys_status['s'] = true; break;
	case 'S': normal_keys_status['S'] = true; break;
		
	case 'a': normal_keys_status['a'] = true; break;
	case 'A': normal_keys_status['A'] = true; break;
		
	case 'd': normal_keys_status['d'] = true; break;
	case 'D': normal_keys_status['D'] = true; break;
		
	case 'q': normal_keys_status['q'] = true; break;
	case 'Q': normal_keys_status['Q'] = true; break;

	case 'e': normal_keys_status['e'] = true; break;
	case 'E': normal_keys_status['E'] = true; break;
		
	case 'r': 
	case 'R': 
		teapot_is_rotate_bool = !teapot_is_rotate_bool;
		break;
	case 'm':
	case 'M':
		render_way++;
		if (render_way >= 3) render_way = 0;
		break;
	case '/':
		show_axes = !show_axes;
		break;
	case 'x':
	case 'X':
		break;
	case 'y':
	case 'Y':
		break;
	case 'z':
	case 'Z':
		break;
	}

}
void processSpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		special_keys_status[GLUT_KEY_LEFT] = true;
		break;
	case GLUT_KEY_RIGHT:
		special_keys_status[GLUT_KEY_RIGHT] = true;
		break;
	case GLUT_KEY_UP:
		special_keys_status[GLUT_KEY_UP] = true;
		break;
	case GLUT_KEY_DOWN:
		special_keys_status[GLUT_KEY_DOWN] = true;
		break;
	case GLUT_KEY_PAGE_UP:
		break;
	case GLUT_KEY_PAGE_DOWN:
		break;
	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
		break;
	}
}

void releaseNormalKeys(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		glutDestroyMenu(main_menu);
		glutDestroyMenu(axes_menu);
		glutDestroyMenu(fill_menu);
		exit(0); 
		break; //key ESC
	case 'w': normal_keys_status['w'] = false; break;
	case 'W': normal_keys_status['W'] = false; break;
	case 's': normal_keys_status['s'] = false; break;
	case 'S': normal_keys_status['S'] = false; break;
	case 'a': normal_keys_status['a'] = false; break;
	case 'A': normal_keys_status['A'] = false; break;
	case 'd': normal_keys_status['d'] = false; break;
	case 'D': normal_keys_status['D'] = false; break;
	case 'q': normal_keys_status['q'] = false; break;
	case 'Q': normal_keys_status['Q'] = false; break;
	case 'e': normal_keys_status['e'] = false; break;
	case 'E': normal_keys_status['E'] = false; break;
	}
}
void releaseSpacialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		special_keys_status[GLUT_KEY_LEFT] = false;
		break;
	case GLUT_KEY_RIGHT:
		special_keys_status[GLUT_KEY_RIGHT] = false;
		break;
	case GLUT_KEY_UP:
		special_keys_status[GLUT_KEY_UP] = false;
		break;
	case GLUT_KEY_DOWN:
		special_keys_status[GLUT_KEY_DOWN] = false;
		break;
	case GLUT_KEY_F1:
		glutFullScreen();
		break;
	case GLUT_KEY_F2:
		window_width = 800;
		window_height = 800;
		glutReshapeWindow(window_width, window_height);
		break;
	case GLUT_KEY_PAGE_UP:
		break;
	case GLUT_KEY_PAGE_DOWN:
		break;
	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
		break;
	}
}

void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {
		// when the buttion is released
		if (state == GLUT_UP) {
			camera_angle[0] = 0.f;
			x_origin = -1;
		}
		else { // state == GLUT_DOWN
			x_origin = x;
		}
	}
}
void mouseMove(int x, int y) {
	// this will only be true when the left button is down
	if (x_origin >= 0) {
		mouse_delta_move[0] = (x - x_origin) * mouse_speed;
	}
}

void processMainMenu(int option) {

	// nothing to do in here
	// all actions are for submenus
}
void processFillMenu(int option) {
	switch (option) {
	case 0:
		render_way = 0;
		break;
	case 1:
		render_way = 1;
		break;
	case 2:
		render_way = 2;
		break;
	default:
		break;
	}
}
void processAxesMenu(int option) {
	switch (option) {
	case 0:
		show_axes = true;
		break;
	case 1:
		show_axes = false;
		break;
	default:
		break;
	}
}
void processMenuStatus(int status, int x, int y) {

	//if (status == GLUT_MENU_IN_USE)
		//menuFlag = 1;
	//else
		//menuFlag = 0;
}
void createPopupMenus() {
	fill_menu = glutCreateMenu(processFillMenu);
	glutAddMenuEntry("GL_POINT", 0);
	glutAddMenuEntry("GL_LINE", 1);
	glutAddMenuEntry("GL_FILL", 2);

	axes_menu = glutCreateMenu(processAxesMenu);
	glutAddMenuEntry("Enable", 0);
	glutAddMenuEntry("Disable", 1);

	main_menu = glutCreateMenu(processMainMenu);

	glutAddSubMenu("Polygon Mode", fill_menu);
	glutAddSubMenu("World Axes", axes_menu);
	// attach the menu to the right button
	// cannot attach menus in game mode.
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// this will allow us to know if the menu is active
	// glutMenuStatusFunc(processMenuStatus);

}


//void mouseButton(int button, int state, int x, int y) {
//
//	// only start motion if the left button is pressed
//	if (button == GLUT_LEFT_BUTTON) {
//		// when the buttion is released
//		if (state == GLUT_UP) {
//			deltaAngle.setZero();
//			xOrigin = -1;
//			// yOrigin = -1;
//		}
//		else { // state == GLUT_DOWN
//			xOrigin = x;
//			// yOrigin = y;
//		}
//	}
//}
//void mouseMove(int x, int y) {
//	float speed = 0.005;
//	// this will only be true when the left button is down
//	//if (xOrigin >= 0) {
//	//	// update deltaAngle
//	//	deltaAngle[1] = (x - xOrigin) * speed;
//
//	//	// update camera's direction
//	//	camera.yaw(t-deltaAngle[1]);
//	//}
//
//	if (xOrigin >= 0) {
//		// update deltaAngle
//		
//		deltaAngle[1] = (x - xOrigin) * speed;
//
//		// update camera's direction
//		camera.yaw(-deltaAngle[1]);
//	}
//	//if (yOrigin >= 0) {
//	//	// update deltaAngle
//
//	//	deltaAngle[0] = (y - yOrigin) * speed;
//
//	//	// update camera's direction
//	//	camera.pitch(-deltaAngle[0]);
//	//}
//
//
//
//}

void idle() {
	glutPostRedisplay();//调用当前绘制函数 
}

void init() {
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	glutIgnoreKeyRepeat(1);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialUpFunc(releaseSpacialKeys);
	
	// mouse
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// glClearColor(0.0, 0.0, 0.0, 1.0);//设置清除颜色,黑色背景
}



int main(int argc, char**argv) {

	Eigen::Matrix4f location;
	location << -0.907537, 0.000000, -0.419973, 283.934509,
		-0.189534, 0.892371, 0.409573, -60.423790,
		0.374771, 0.451302, -0.809860, -674.504822,
		0.000000, 0.000000, 0.000000, 1.000000;
	
	OpenMesh::IO::read_mesh(s0, s0_path);
	// camera.SetViewMatrix(location);
	camera.LookAt(0, 0, 10,
			0, 0, 0,
			0, 1, 0);
	//camera.PrintInfo();

	/*camera.setCamera(333.661, 169.086, - 21.2678,
	0, 0, 0,
	0, 1, 0);*/
	
	//camera.setCamera(0, 0, 10,
	//	0, 0, 0,
	//	0, 1, 0);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_width, window_height);
	main_window = glutCreateWindow("Tutorial");


	// callbacks for main window
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderAllScene);
	init();
	
	// init Menus
	// createPopupMenus();

	sub_window[0] = glutCreateSubWindow(main_window, border, border, window_width - 2 * border, window_height - border * 3 / 2);
	glutDisplayFunc(renderSw0Scene);
	init();

	sub_window[1] = glutCreateSubWindow(main_window, border, (window_height + border) / 2, window_width / 2 - border * 3 / 2, window_height / 2 - border * 3 / 2);
	glutDisplayFunc(renderSw1Scene);
	init();

	sub_window[2] = glutCreateSubWindow(main_window, (window_width + border) / 2, (window_height + border) / 2, window_width / 2 - border * 3 / 2, window_height / 2 - border * 3 / 2);
	glutDisplayFunc(renderSw2Scene);
	init();
	

	// enter GLUT event processing cycle
	glutMainLoop();

	system("pause");
	return 0;
}