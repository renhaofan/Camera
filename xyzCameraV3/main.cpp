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

Eigen::Vector3d deltaShift(0, 0, 0);
Eigen::Vector3d deltaAngle(0, 0, 0);
int xOrigin = -1;
int yOrigin = -1;
double xLength = 0;
double yLength = 0;


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
	gluPerspective(45.0f, ratio, 0.1, 1000.0);
	//gluPerspective(45.0f, ratio, 1, 10.0);


	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}
void plotWorldAxis() {
	// plot 3-dim axis
	glLineWidth(5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();
	glLineWidth(1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
}
void plotReferenceGrid(float start = 20.0f, float gridSize = 1.0f) {
	glColor3f(0.5f, 0.5, 0.5); // gray color
	for (float i = -start; i <= start; i++) {
		glBegin(GL_LINES);
		glVertex3f(i*gridSize, -1.0f, -start);
		glVertex3f(i*gridSize, -1.0f, start);
		glVertex3f(-start, -1.0f, i*gridSize);
		glVertex3f(start, -1.0f, i*gridSize);
		glEnd();
	}
	glColor3f(1.0f, 1.0f, 1.0f);
}
void plotMesh(MyMesh* m) {
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
}


void renderScene() {
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	Eigen::Vector3d tmp = camera.GetViewTranslate();
	if (std::abs(deltaShift[2]) > 1e-5) {
		camera.SetViewTranslateMatrix(tmp.x(), tmp.y(), deltaShift[2]);
	}




	/*if (std::abs(deltaShift[0]) > 1e-5) {
		camera.shiftForward(deltaShift[0]);
	}

	if (std::abs(deltaShift[1]) > 1e-5) {
		camera.shiftLeft(deltaShift[1]);
	}

	if (std::abs(deltaShift[2]) > 1e-5) {
		camera.shiftUp(deltaShift[2]);
	}

	if (std::abs(deltaAngle[0]) > 1e-5) {
		camera.pitch(-deltaAngle[0]);
	}

	if (std::abs(deltaAngle[1]) > 1e-5) {
		camera.yaw(-deltaAngle[1]);
	}*/


	 //Set the camera
	//gluLookAt(10, 10, 10,
	//	0, 0, 0,
	//	0, 1, 0);

	// camera.setModelViewMatrix();
	// camera.printSelf();

	//double* m = camera.getMV();
	//
	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixd(m);
	Eigen::Matrix4d matrixModelView = camera.GetViewMatrix();
	double m[16];
	m[0] = matrixModelView(0, 0); m[4] = matrixModelView(0, 1);  m[8] = matrixModelView(0, 2);  m[12] = matrixModelView(0, 3);
	m[1] = matrixModelView(1, 0); m[5] = matrixModelView(1, 1);  m[9] = matrixModelView(1, 2);  m[13] = matrixModelView(1, 3);
	m[2] = matrixModelView(2, 0); m[6] = matrixModelView(2, 1);  m[10] = matrixModelView(2, 2); m[14] = matrixModelView(2, 3);
	m[3] = matrixModelView(3, 0); m[7] = matrixModelView(3, 1);  m[11] = matrixModelView(3, 2);  m[15] = matrixModelView(3, 3);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m);

	float mat[16];  //按照列存储，所以按照行打出来
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	for (int index = 0; index < 4; index++) {
		for (int i = 0; i < 4; ++i) {
			printf("%f ", mat[index + i * 4]);
		}
		cout << endl;
	}
	cout << endl;



	// plot reference grid
	plotReferenceGrid();
	// plot world coordinate axis
	plotWorldAxis();
	// plot teapot
	glColor3f(1.0, 1.0, 1.0);
	//glutSolidTeapot(1.0f);
	glutWireTeapot(1.0f);
	
	// plot mesh
	plotMesh(&s0);
	


	



	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
	float speed = 0.01;
	switch (key) {
	case 27: exit(0); break; //key ESC
	case 'x':
	case 'X': 

		break;
	case 'y':
	case 'Y': 

		break;
	case 'z':
	case 'Z': 

		break;

	case 'w':
	case 'W':
		camera.ShiftForward(0.1);
		break;
	case 's':
	case 'S':
		camera.ShiftForward(-0.1);
		break;
	case 'a':
	case 'A':
		camera.ShiftLeft(0.1);
		break;
	case 'd':
	case 'D':
		camera.ShiftLeft(-0.1);
		break;
	case 'q':
	case 'Q':
		camera.ShiftUp(0.1);
		break;
	case 'e':
	case 'E':
		camera.ShiftUp(-0.1);
		break;
	}

}
void processSpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		deltaAngle[1] = -0.5f;
		break;
	case GLUT_KEY_RIGHT:
		deltaAngle[1] = 0.5f;
		break;
	case GLUT_KEY_UP:
		deltaAngle[0] = 0.5f;
		break;
	case GLUT_KEY_DOWN:
		deltaAngle[0] = -0.5f;
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
		deltaShift[0] = 0.f;
		break;
	case 'a':
	case 'A':
	case 'd':
	case 'D':
		deltaShift[1] = 0.f;
		break;
	case 'q':
	case 'Q':
	case 'e':
	case 'E':
		deltaShift[2] = 0.f;
		break;
	}
}
void releaseSpacialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
		deltaAngle[1] = 0.f;
		break;

	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		deltaAngle[0] = 0.f;
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
	//glClearColor(1.0, 1.0, 1.0, 0.0);//设置清除颜色
	//glClear(GL_COLOR_BUFFER_BIT);//把窗口清除为当前颜色
}





int main(int argc, char**argv) {

	Eigen::Matrix4d location;
	location << -0.907537, 0.000000, -0.419973, 283.934509,
		-0.189534, 0.892371, 0.409573, -60.423790,
		0.374771, 0.451302, -0.809860, -674.504822,
		0.000000, 0.000000, 0.000000, 1.000000;
	
	Eigen::Vector3d tu(1, 0, -1);
	Eigen::Vector3d tv(0, 1, 1);
	Eigen::Vector3d tw(1, 0, 1);
	Eigen::Vector3d te(-1, 0, -1);


	OpenMesh::IO::read_mesh(s0, s0_path);
	//camera.SetViewMatrix(tu, tv, tw, te);
	//camera.SetViewMatrix(location);
	camera.LookAt(10, 0, 10,
			0, 0, 0,
			0, 1, 0);
	camera.PrintInfo();

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