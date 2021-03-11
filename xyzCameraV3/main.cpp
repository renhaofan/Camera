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

GLCamera camera;
// camera concerned
float camera_position[3] = {0.f, 0.f, -10.f};
int xOrigin = -1;
int yOrigin = -1;
double xLength = 0;
double yLength = 0;

// teapot
float teapot_rotate = 0.f;
// render way LINE/FILL
int render_way = 1;
bool teapot_is_rotate_bool = false;


void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	
	gluPerspective(45.0f, ratio, 0.1, 5000.0);
	




	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


void renderBitmapString(float x, float y, float z, void *font, char *string) {
	// example: renderBitmapString(1.0f, 0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"X");
	char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
void plotWorldAxis() {
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

	renderBitmapString(half_length + 0.3f, 0.f, 0.f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"X");
	renderBitmapString(0.f, half_length + 0.3f, 0.f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"Y");
	renderBitmapString(0.f, 0.f, half_length + 0.3f, GLUT_BITMAP_TIMES_ROMAN_24, (char*)"Z");

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

void renderScene() {
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();


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
		

	Eigen::Matrix4f matrixModelView = camera.GetViewMatrix();
	double m[16];
	m[0] = matrixModelView(0, 0); m[4] = matrixModelView(0, 1);  m[8] = matrixModelView(0, 2);  m[12] = matrixModelView(0, 3);
	m[1] = matrixModelView(1, 0); m[5] = matrixModelView(1, 1);  m[9] = matrixModelView(1, 2);  m[13] = matrixModelView(1, 3);
	m[2] = matrixModelView(2, 0); m[6] = matrixModelView(2, 1);  m[10] = matrixModelView(2, 2); m[14] = matrixModelView(2, 3);
	m[3] = matrixModelView(3, 0); m[7] = matrixModelView(3, 1);  m[11] = matrixModelView(3, 2);  m[15] = matrixModelView(3, 3);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m);

	


	//float mat[16];  //按照列存储，所以按照行打出来
	//glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	//for (int index = 0; index < 4; index++) {
	//	for (int i = 0; i < 4; ++i) {
	//		printf("%f ", mat[index + i * 4]);
	//	}
	//	cout << endl;
	//}
	//cout << endl;



	// plot reference grid
	plotReferenceGrid(20.f);
	// plot world coordinate axis
	plotWorldAxis();
	plotCamera(camera_position);

	if (teapot_is_rotate_bool) teapot_rotate += 1.f;

	//GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 }; // 定义颜色
	//GLfloat light_pos[] = { 5,5,5,1 };  //定义光源位置
	//glLightfv(GL_LIGHT0, GL_POSITION, light_pos); //设置第0号光源的光照位置
	//glLightfv(GL_LIGHT0, GL_AMBIENT, white); //设置第0号光源多次反射后的光照颜色（环境光颜色）
	//
	//										 
	//glEnable(GL_LIGHTING); //开启光照模式
	//glEnable(GL_LIGHT0); //开启第0号光源
	//plotTeaTable();
	//glDisable(GL_LIGHT0);
	//glDisable(GL_LIGHTING);

	// plot mesh
	plotMesh(&s0);
	
	
	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
	float speed = 1;
	switch (key) {
	case 27: exit(0); break; //key ESC
	case 'w':
	case 'W':
		camera.ShiftForward(0.1 * speed);
		break;
	case 's':
	case 'S':
		camera.ShiftBackward(0.1 * speed);
		break;
	case 'a':
	case 'A':
		camera.ShiftLeft(0.1 * speed);
		break;
	case 'd':
	case 'D':
		camera.ShiftLeft(-0.1 * speed);
		break;
	case 'q':
	case 'Q':
		camera.ShiftUp(0.1 * speed);
		break;
	case 'e':
	case 'E':
		camera.ShiftUp(-0.1 * speed);
		break;
	case 'r':
	case 'R':
		teapot_is_rotate_bool = !teapot_is_rotate_bool;
		break;
	case 'm':
	case 'M':
		render_way++;
		if (render_way >= 3) render_way = 0;
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
		camera.YawV(-0.1f);
		break;
	case GLUT_KEY_RIGHT:
		camera.YawV(0.1f);
		break;
	case GLUT_KEY_UP:
		camera.PitchU(-0.1f);
		break;
	case GLUT_KEY_DOWN:
		camera.PitchU(0.1f);
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
	case 'w':
	case 'W':
	case 's':
	case 'S':
	
		break;
	case 'a':
	case 'A':
	case 'd':
	case 'D':

		break;
	case 'q':
	case 'Q':
	case 'e':
	case 'E':
	
		break;
	}
}
void releaseSpacialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
	
		break;

	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
	
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
void SetRC() {
	glEnable(GL_DEPTH_TEST);
	// glClearColor(0.0, 0.0, 0.0, 1.0);//设置清除颜色,黑色背景
	//glClear(GL_COLOR_BUFFER_BIT);//把窗口清除为当前颜色
}





int main(int argc, char**argv) {

	Eigen::Matrix4f location;
	location << -0.907537, 0.000000, -0.419973, 283.934509,
		-0.189534, 0.892371, 0.409573, -60.423790,
		0.374771, 0.451302, -0.809860, -674.504822,
		0.000000, 0.000000, 0.000000, 1.000000;
	
	OpenMesh::IO::read_mesh(s0, s0_path);
	// camera.SetViewMatrix(location);
	camera.LookAt(0, 10, 0,
			0, 0, 0,
			0, 0, 1);
	//camera.PrintInfo();

	/*camera.setCamera(333.661, 169.086, - 21.2678,
	0, 0, 0,
	0, 1, 0);*/
	
	//camera.setCamera(0, 0, 10,
	//	0, 0, 0,
	//	0, 1, 0);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1024, 800);
	glutCreateWindow(argv[0]);

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(idle);  

	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	//glutIgnoreKeyRepeat(1);
	/*glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialUpFunc(releaseSpacialKeys);*/

	// mouse
	//glutMouseFunc(mouseButton);
	//glutMotionFunc(mouseMove);

	// OpenGL init
	SetRC();

	glutMainLoop();

	system("pause");
	return 0;
}