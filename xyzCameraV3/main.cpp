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

#include "Mesh.hpp"

const float DEG2RAD = 3.141593f / 180.0f;
const float RAD2DEG = 180.0f / 3.141593f;
const float EPSILON = 0.00001f;

// reference: http://www.songho.ca/opengl/gl_camera.html#lookat

#include "GLCamera.hpp"

//class GLCamera {
//	//　matrix store element is row-major
//public:
//	GLCamera();
//	~GLCamera();
//
//	// OpenGL concerned
//	void init();    // initialize OpenGL states
//	void quit();    // clean up OpenGL objects
//
//	// print position, target, ModelViewMatrix
//	void printSelf();
//
//	// only change camera position, target position is const
//	void moveTo(const Eigen::Vector3d &to);
//	void moveTo(double tx, double ty, double tz) { moveTo(Eigen::Vector3d(tx, ty, tz)); };
//	void moveForward(double delta);
//	void moveBackword(double delta) { moveForward(-delta); };
//
//	// shift both camera position and target position in same direction; left/right/up/down
//	void shiftTo(const Eigen::Vector3d& to);
//	void shiftTo(double tx, double ty, double tz) { shiftTo(Eigen::Vector3d(tx, ty, tz)); };
//	void shift(const Eigen::Vector3d& delta);
//	void shift(double deltaRight, double deltaUp, double backward) { shift(Eigen::Vector3d(deltaRight, deltaUp, backward)); };
//	
//	void shiftLeft(double deltaLeft) { if (deltaLeft <= 0) return;  shift(-deltaLeft, 0, 0); };
//	void shiftRight(double deltaRight) { if (deltaRight <= 0) return;  shift(deltaRight, 0, 0); };
//	void shiftUp(double deltaUp) { if (deltaUp <= 0) return;  shift(0, deltaUp, 0); };
//	void shiftDown(double deltaDown) { if (deltaDown <= 0) return;  shift(0, -deltaDown, 0); };
//	void shiftForward(double deltaForward) { if (deltaForward <= 0) return;  shift(0, 0, -deltaForward); };
//	void shiftBackward(double deltaBackward) { if (deltaBackward <= 0) return;  shift(0, 0, deltaBackward); };
//
//
//
//	// rotate
//	void pitch(double degree);
//	void yaw(double degree);
//	void roll(double degree);
//
//	// zoom camera
//	void zoomCameraDelta(double delta);
//
//
//	// setters 
//	void setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar);
//	void setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar, Eigen::Vector3d upDir);
//	void setCamera(double px, double py, double pz, double tx, double ty, double tz) {
//		setCamera(Eigen::Vector3d(px, py, pz), Eigen::Vector3d(tx, ty, tz));
//	}
//	void setCamera(double px, double py, double pz, double tx, double ty, double tz, double ux, double uy, double uz) {
//		setCamera(Eigen::Vector3d(px, py, pz), Eigen::Vector3d(tx, ty, tz), Eigen::Vector3d(ux, uy, uz));
//	}
//	
//	
//	void setModelViewMatrix(const Eigen::Matrix4d& m) { matrixModelView = m; };
//	void setModelMatrix(const Eigen::Matrix4d& m) { matrixModel = m; }
//	void setViewMatrix(const Eigen::Matrix4d& m) { matrixView = m; }
//	void setViewRotateMatrix(const Eigen::Matrix4d& m) { matrixViewRotate = m; }
//	void setViewTranslateMatrix(const Eigen::Matrix4d& m) { matrixViewTranslate = m; }
//	void setProjectionMatrix();
//
//
//	// getters
//	Eigen::Vector3d getCameraPosition() { return cameraPosition; };
//	Eigen::Vector3d getTarget() { return target; };
//	Eigen::Vector3d getCameraAngle() { return cameraAngle; };
//	Eigen::Matrix4d getMatrixView() { return matrixView; };
//	Eigen::Matrix4d getMatrixViewTranslate() { return matrixViewTranslate; };
//	Eigen::Matrix4d getMatrixViewRotate() { return matrixViewRotate; };
//	Eigen::Matrix4d getMatrixModel() { return matrixModel; };
//	Eigen::Matrix4d getMatrixModelView() { return matrixModelView; };
//	Eigen::Matrix4d getMatrixProjection() { return matrixProjection; };
//	double getDistance() { return distance; }
//
//
//	// update
//	void updateDistance() { distance = (target - cameraPosition).norm(); }
//	void updateViewMatrix() { matrixView = matrixViewRotate * matrixViewTranslate; }
//	void updateModelViewMatrix() { matrixModelView = matrixView * matrixModel; }
//
//	// to vector homogeneous coordinates
//	Eigen::Vector4d toVector4d(const Eigen::Vector3d& v) {
//		return Eigen::Vector4d(v.x(), v.y(), v.z(), 0);
//	}
//
//	// mouse control
//	void setMousePosition(int x, int y) { mouseX = x; mouseY = y; };
//
//protected:
//
//private:
//	// Camera
//	Eigen::Vector3d cameraPosition;   // camera position at world space
//	Eigen::Vector3d target;			  // camera focal(lookat), i.e target, position at world space
//	double distance;                  // distance between cameraPosition and target
//	Eigen::Vector3d cameraAngle;      // pitch(X),heading(Y), Roll(Z) around self coordinate
//
//
//
//	// 4x4 transform matrices
//	Eigen::Matrix4d matrixView;
//	Eigen::Matrix4d matrixViewRotate; // x,y,z  row-major
//	Eigen::Matrix4d matrixViewTranslate;
//	Eigen::Matrix4d matrixModel;
//	Eigen::Matrix4d matrixModelView;
//	Eigen::Matrix4d matrixProjection;
//
//
//	int mouseX; int mouseY;
//	double cameraDistance;
//
//};
//
//
//GLCamera::GLCamera() {
//
//	this->matrixViewRotate.setIdentity();
//	this->matrixViewTranslate.setIdentity();
//	this->matrixView = matrixViewRotate * matrixViewTranslate;
//
//	this->matrixModel.setIdentity();
//	this->matrixModelView = matrixView * matrixModel;
//	this->matrixProjection.setIdentity();
//
//
//
//	this->mouseX = this->mouseY = 0;
//	this->cameraDistance = 0;
//	this->cameraPosition = Eigen::Vector3d(0, 0, 0);
//	this->target = Eigen::Vector3d(0, 0, 0); // this means target, not the real focal;
//	this->distance = (target - cameraPosition).norm();
//	this->cameraAngle = Eigen::Vector3d(0, 0, 0);
//
//	// setModelViewMatrix();
//	//this->intrinsics.setIdentity();
////	this->extrinsics.setIdentity();
//
//}
//
//GLCamera::~GLCamera() {
//
//}
//
/////////////////////////////////////////////////////////////////////////////////
//// initialize OpenGL states and scene
/////////////////////////////////////////////////////////////////////////////////
//void GLCamera::init() {
//	glEnable(GL_DEPTH_TEST);
//	//glClearColor(1.0, 1.0, 1.0, 0.0);//设置清除颜色
//	//glClear(GL_COLOR_BUFFER_BIT);//把窗口清除为当前颜色
//
//}
//
/////////////////////////////////////////////////////////////////////////////////
//// clean up OpenGL objects
/////////////////////////////////////////////////////////////////////////////////
//void GLCamera::quit() {
//
//}
//
//void GLCamera::printSelf() {
//	std::cout << "====GLCamera======\n"
//		<< "  Position.T(): " << cameraPosition.transpose() << "\n"
//		<< "    Target.T():   " << target.transpose() << "\n"
//		<< "    matrixModelView:     \n" << matrixModelView << "\n"
//		<< "  Distance: " << this->distance << std::endl;
//}
//
//void GLCamera::zoomCameraDelta(double delta) {
//	if (delta > 0) this->shiftForward(delta);
//	else this->shiftBackward(-delta);
//}
//
//
//#pragma region(shift)
//void GLCamera::shiftTo(const Eigen::Vector3d& to) {
//	Eigen::Vector3d forward = to - cameraPosition;
//	Eigen::Vector3d newTarget = target + forward;
//
//
//	this->cameraPosition = to;
//	this->target = newTarget;
//	
//	updateDistance();
//	
//	this->matrixViewTranslate.block<3, 1>(0, 3) = -to;
//	updateViewMatrix();
//	updateModelViewMatrix();
//}
//
// 
//// delta > 0, right, up, backward;
//void GLCamera::shift(const Eigen::Vector3d& delta) {
//
//	Eigen::Matrix4d m = matrixViewRotate.transpose();
//
//
//	Eigen::Vector3d deltaRight = delta(0) * m.block<3, 1>(0, 0);
//	Eigen::Vector3d deltaUp = delta(1) * m.block<3, 1>(0, 1);
//	Eigen::Vector3d deltaBackward = delta(2) * m.block<3, 1>(0, 2);
//
//	Eigen::Vector3d newPosition = cameraPosition + deltaRight + deltaUp + deltaBackward;
//
//	shiftTo(newPosition);
//}
//#pragma endregion(shift)
//
//#pragma region(rotate)
//void GLCamera::pitch(double degree) {
//	Eigen::Matrix4d rotate;
//	rotate.setIdentity();
//	rotate(1, 1) = std::cos(degree * DEG2RAD);
//	rotate(2, 2 ) = std::cos(degree * DEG2RAD);
//	rotate(1, 2) = -std::sin(degree * DEG2RAD);
//	rotate(2, 1) = std::sin(degree * DEG2RAD);
//
//	matrixViewRotate *= rotate;
//	updateViewMatrix();
//	updateModelViewMatrix();
//
//}
//void GLCamera::yaw(double degree) {
//	Eigen::Matrix4d rotate;
//	degree = -degree;
//	rotate.setIdentity();
//	rotate(0, 0) = std::cos(degree * DEG2RAD);
//	rotate(2, 2) = std::cos(degree * DEG2RAD);
//	rotate(2, 0) = -std::sin(degree * DEG2RAD);
//	rotate(0, 2) = std::sin(degree * DEG2RAD);
//
//	matrixViewRotate *= rotate;
//
//
//	updateViewMatrix();
//	updateModelViewMatrix();
//}
//void GLCamera::roll(double degree) {
//	Eigen::Matrix4d rotate;
//	rotate.setIdentity();
//	rotate(0, 0) = std::cos(degree * DEG2RAD);
//	rotate(1, 1) = std::cos(degree * DEG2RAD);
//	rotate(1, 0) = std::sin(degree * DEG2RAD);
//	rotate(0, 1) = -std::sin(degree * DEG2RAD);
//
//	matrixViewRotate *= rotate;
//	updateViewMatrix();
//	updateModelViewMatrix();
//}
//
//#pragma endregion(rotate)
//
//#pragma region(setCamera)
//void GLCamera::setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar) {
//	this->cameraPosition = pos;
//	this->target = tar;
//	updateDistance();
//
//	this->matrixViewTranslate.block<3, 1>(0, 3) = -pos;
//
//
//
//	Eigen::Vector3d w = pos - tar; w.normalize();
//	Eigen::Vector3d v;
//
//
//	if (abs(this->cameraPosition.x()) < EPSILON && abs(this->cameraPosition.z()) < EPSILON) {
//		if (w.y() > 0) {
//			v = Eigen::Vector3d(0, 0, -1);
//		}
//		else {
//			v = Eigen::Vector3d(0, 0, 1);
//		}
//	}
//	else
//	{
//		v = Eigen::Vector3d(0, 1, 0);
//	}
//
//	Eigen::Vector3d u = v.cross(w); u.normalize();
//	// recompute up vector, make three axis orthometric
//	v = w.cross(u); v.normalize();
//
//	Eigen::Matrix4d uvw; uvw.setIdentity();
//	uvw.block<3, 1>(0, 0) = u;
//	uvw.block<3, 1>(0, 1) = v;
//	uvw.block<3, 1>(0, 2) = w;
//
//
//
//	this->matrixViewRotate = uvw.transpose();
//	updateViewMatrix();
//	updateModelViewMatrix();
//}
//
//
//void GLCamera::setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar, Eigen::Vector3d upDir) {
//	this->cameraPosition = pos;
//	this->target = tar;
//	updateDistance();
//
//	this->matrixViewTranslate.block<3, 1>(0, 3) = -pos;
//
//
//
//	Eigen::Vector3d w = pos - tar; w.normalize();
//	Eigen::Vector3d v = upDir;
//
//
//	Eigen::Vector3d u = v.cross(w); u.normalize();
//	// recompute up vector, make three axis orthometric
//	v = w.cross(u); v.normalize();
//
//	Eigen::Matrix4d uvw; uvw.setIdentity();
//	uvw.block<3, 1>(0, 0) = u;
//	uvw.block<3, 1>(0, 1) = v;
//	uvw.block<3, 1>(0, 2) = w;
//
//
//
//	this->matrixViewRotate = uvw.transpose();
//	updateViewMatrix();
//	updateModelViewMatrix();
//}
//#pragma endregion(setCamera)



GLCamera camera;
std::string s0 = "s0.obj";
Mesh mesh;
Eigen::Vector3d deltaShift(0, 0, 0);

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
}
void drawMesh(Mesh& m) {
	if (m.faceNum == 0 || m.vetexNum == 0) {
		throw std::runtime_error("Invalid Mesh");
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < m.face.size(); i++) {
		auto faceId = m.face[i];
		auto vetexId1 = faceId[0] - 1;
		auto vetexId2 = faceId[1] - 1;
		auto vetexId3 = faceId[2] - 1;
		glBegin(GL_LINE_STRIP);
		glVertex3f(m.vetex[vetexId1][0]-m.centroid[0], m.vetex[vetexId1][1]-m.centroid[1], m.vetex[vetexId1][2]-m.centroid[2]);
		glVertex3f(m.vetex[vetexId2][0]-m.centroid[0], m.vetex[vetexId2][1]-m.centroid[1], m.vetex[vetexId2][2]-m.centroid[2]);
		glVertex3f(m.vetex[vetexId3][0]-m.centroid[0], m.vetex[vetexId3][1]-m.centroid[1], m.vetex[vetexId3][2]-m.centroid[2]);
		glEnd();
	}
}

void drawTest() {
	glLineWidth(5.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	for (size_t j = 0; j < 3; j++) {
		glVertex3f(j, 0.0f, 0.0f);
		glVertex3f(0.0f, j, 0.0f);

	}
	glEnd();
	glLineWidth(1.0f);
}
//void drawMesh(Mesh& m) {
//	if (m.faceNum == 0 || m.vetexNum == 0) {
//		throw std::runtime_error("Invalid Mesh");
//	}
//	glColor3f(1.0f, 1.0f, 1.0f);
//	for (size_t i = 0; i < m.triangles.size(); i++) {
//		Eigen::Vector3d v;
//		glBegin(GL_LINE_STRIP);
//		for (size_t j = 0; j < 3; j++) {
//			v = m.triangles[i].v[j];
//			glVertex3d(v[0] - 330, v[1] - 165, v[2] + 20);
//		}
//		glEnd();
//	}
//}


void renderScene() {
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	if (std::abs(deltaShift[0]) > 1e-5) {
		camera.shiftForward(deltaShift[0]);
	}

	if (std::abs(deltaShift[1]) > 1e-5) {
		camera.shiftLeft(deltaShift[1]);
	}

	if (std::abs(deltaShift[2]) > 1e-5) {
		camera.shiftUp(deltaShift[2]);
	}



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
	Eigen::Matrix4d matrixModelView = camera.getMatrixModelView();
	double m[16];
	m[0] = matrixModelView(0, 0); m[4] = matrixModelView(0, 1);  m[8] = matrixModelView(0, 2);  m[12] = matrixModelView(0, 3);
	m[1] = matrixModelView(1, 0); m[5] = matrixModelView(1, 1);  m[9] = matrixModelView(1, 2);  m[13] = matrixModelView(1, 3);
	m[2] = matrixModelView(2, 0); m[6] = matrixModelView(2, 1);  m[10] = matrixModelView(2, 2); m[14] = matrixModelView(2, 3);
	m[3] = matrixModelView(3, 0); m[7] = matrixModelView(3, 1);  m[11] = matrixModelView(3, 2);  m[15] = matrixModelView(3, 3);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m);
	camera.printSelf();


	//float mat[16];  //按照列存储，所以按照列打出来
	//glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	//for (int index = 0; index < 4; index++) {
	//	for (int i = 0; i < 4; ++i) {
	//		printf("%f ", mat[index + i * 4]);
	//	}
	//	cout << endl;
	//}
	//cout << endl;
	
	plotReferenceGrid();
	// plot world coordinate axis
	plotWorldAxis();
	// plot teapot
	glColor3f(1.0, 1.0, 1.0);
	//glScalef(2.0, 1.0, 1.0);
	glutWireTeapot(1.0f);
	// plot mesh s0
	// drawMesh(mesh);

	// drawTest();
	glutSwapBuffers();
}


void processNormalKeys(unsigned char key, int x, int y) {

	switch (key) {
	case 27: exit(0); break; //key ESC
	case 'x':
	case 'X': 
		camera.pitch(10);
		break;
	case 'y':
	case 'Y': 
		camera.yaw(10);
		break;
	case 'z':
	case 'Z': 
		camera.roll(10);
		break;

	case 'w':
	case 'W':
		deltaShift[0] = 0.1f;
		break;
	case 's':
	case 'S':
		deltaShift[0] = -0.1f;
		break;
	case 'a':
	case 'A':
		deltaShift[1] = 0.1f;
		break;
	case 'd':
	case 'D':
		deltaShift[1] = -0.1f;
		break;
	case 'q':
	case 'Q':
		deltaShift[2] = 0.1f;
		break;
	case 'e':
	case 'E':
		deltaShift[2] = -0.1f;
		break;
	}

}
void processSpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		camera.shiftLeft(1);
		//camera.shift(1, 0);
		break;
	case GLUT_KEY_RIGHT:
		camera.shiftRight(1);
		//camera.shift(-1, 0);
		break;
	case GLUT_KEY_UP:
		camera.shiftUp(1);
		//camera.shift(0, 1);
		break;
	case GLUT_KEY_DOWN:
		camera.shiftDown(1);
		//camera.shift(0, -1);
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




void idle() {
	glutPostRedisplay();//调用当前绘制函数 
}

void SetRC() {
	glEnable(GL_DEPTH_TEST);
	//glClearColor(1.0, 1.0, 1.0, 0.0);//设置清除颜色
	//glClear(GL_COLOR_BUFFER_BIT);//把窗口清除为当前颜色
}


int main(int argc, char**argv) {
	camera.setCamera(0, 0, 10,
		0, 0, 0,
		0, 1, 0);


		//Eigen::Matrix3d subInverse; subInverse.setIdentity();
		//std::cout << subInverse << std::endl;
		//subInverse.col(0) = Eigen::Vector3d(2, 3, 4);
		//std::cout << subInverse.col(0)[1] << std::endl;
	//float x[3] = {1.f, 2.f, 3.f};
	//float * t = x;
	//Eigen::Vector3f out(x);
	//std::cout << out.transpose() << std::endl;



	//mesh.readMesh(s0);
	//std::cout << mesh.radius << std::endl;
	//Eigen::Vector3d position = mesh.centroid + mesh.radius;
	//Eigen::Vector3d target = mesh.centroid;
	//Eigen::Vector3d up = Eigen::Vector3d(0.f, 1.f, 0.f);
	//camera.setCamera(position, target, up);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1024, 800);
	glutCreateWindow(argv[0]);

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(idle);//注册全局回调函数：空闲时调用  


	

	
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	glutIgnoreKeyRepeat(1);
	glutKeyboardUpFunc(releaseNormalKeys);

	/*glutKeyboardUpFunc(NormalKeys_UP);
	glutSpecialUpFunc(SpecialKeys_UP);
*/





	//// here are the new entries
	//glutIgnoreKeyRepeat(1);
	//glutSpecialUpFunc(releaseKey); // process key up event



	// OpenGL init
	SetRC();

	glutMainLoop();


	system("pause");
	return 0;
}