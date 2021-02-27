#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>


class GLCamera {
	//¡¡matrix store element is row-major
public:
	GLCamera();
	~GLCamera();

	// OpenGL concerned
	void init();    // initialize OpenGL states
	void quit();    // clean up OpenGL objects

	// print position, target, ModelViewMatrix
	void printSelf();

	// only change camera position, target position is const
	void moveTo(const Eigen::Vector3d &to);
	void moveTo(double tx, double ty, double tz) { moveTo(Eigen::Vector3d(tx, ty, tz)); };
	void moveForward(double delta);
	void moveBackword(double delta) { moveForward(-delta); };

	// shift both camera position and target position in same direction; left/right/up/down
	void shiftTo(const Eigen::Vector3d& to);
	void shiftTo(double tx, double ty, double tz) { shiftTo(Eigen::Vector3d(tx, ty, tz)); };
	// delta > 0, shift direction---right, up, backward;
	void shift(const Eigen::Vector3d& delta);
	void shift(double deltaRight, double deltaUp, double backward) { shift(Eigen::Vector3d(deltaRight, deltaUp, backward)); };

	void shiftLeft(double deltaLeft);
	void shiftRight(double deltaRight);
	void shiftUp(double deltaUp);
	void shiftDown(double deltaDown);
	void shiftForward(double deltaForward);
	void shiftBackward(double deltaBackward);

	//void shiftLeft(double deltaLeft) { if (deltaLeft <= 0) return;   shift(-deltaLeft, 0, 0); };
	//void shiftRight(double deltaRight) { if (deltaRight <= 0) return;   shift(deltaRight, 0, 0); };
	//void shiftUp(double deltaUp) { if (deltaUp <= 0) return;  shift(0, deltaUp, 0); };
	//void shiftDown(double deltaDown) { if (deltaDown <= 0) return;  shift(0, -deltaDown, 0); };
	//void shiftForward(double deltaForward) { if (deltaForward <= 0) return;  shift(0, 0, -deltaForward); };
	//void shiftBackward(double deltaBackward) { if (deltaBackward <= 0) return;  shift(0, 0, deltaBackward); };



	// rotate
	void pitch(double degree);
	void yaw(double degree);
	void roll(double degree);

	// zoom camera
	void zoomCameraDelta(double delta);


	// setters 
	void setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar);
	void setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar, Eigen::Vector3d upDir);
	void setCamera(double px, double py, double pz, double tx, double ty, double tz) {
		setCamera(Eigen::Vector3d(px, py, pz), Eigen::Vector3d(tx, ty, tz));
	}
	void setCamera(double px, double py, double pz, double tx, double ty, double tz, double ux, double uy, double uz) {
		setCamera(Eigen::Vector3d(px, py, pz), Eigen::Vector3d(tx, ty, tz), Eigen::Vector3d(ux, uy, uz));
	}


	void setModelViewMatrix(const Eigen::Matrix4d& m) { matrixModelView = m; };
	void setModelMatrix(const Eigen::Matrix4d& m) { matrixModel = m; }
	void setViewMatrix(const Eigen::Matrix4d& m) { matrixView = m; }
	void setViewRotateMatrix(const Eigen::Matrix4d& m) { matrixViewRotate = m; }
	void setViewTranslateMatrix(const Eigen::Matrix4d& m) { matrixViewTranslate = m; }
	void setProjectionMatrix();


	// getters
	Eigen::Vector3d getCameraPosition() { return cameraPosition; };
	Eigen::Vector3d getTarget() { return target; };
	Eigen::Vector3d getCameraAngle() { return cameraAngle; };
	Eigen::Matrix4d getMatrixView() { return matrixView; };
	Eigen::Matrix4d getMatrixViewTranslate() { return matrixViewTranslate; };
	Eigen::Matrix4d getMatrixViewRotate() { return matrixViewRotate; };
	Eigen::Matrix4d getMatrixModel() { return matrixModel; };
	Eigen::Matrix4d getMatrixModelView() { return matrixModelView; };
	Eigen::Matrix4d getMatrixProjection() { return matrixProjection; };
	double getDistance() { return distance; }


	// update
	void updateDistance() { distance = (target - cameraPosition).norm(); }
	void updateViewMatrix() { matrixView = matrixViewRotate * matrixViewTranslate; }
	void updateModelViewMatrix() { matrixModelView = matrixView * matrixModel; }

	// to vector homogeneous coordinates
	Eigen::Vector4d toVector4d(const Eigen::Vector3d& v) {
		return Eigen::Vector4d(v.x(), v.y(), v.z(), 0);
	}

	// mouse control
	void setMousePosition(int x, int y) { mouseX = x; mouseY = y; };

protected:

private:
	// Camera
	Eigen::Vector3d cameraPosition;   // camera position at world space
	Eigen::Vector3d target;			  // camera focal(lookat), i.e target, position at world space
	double distance;                  // distance between cameraPosition and target
	Eigen::Vector3d cameraAngle;      // pitch(X),heading(Y), Roll(Z) around self coordinate



	// 4x4 transform matrices
	Eigen::Matrix4d matrixView;
	Eigen::Matrix4d matrixViewRotate; // x,y,z  row-major
	Eigen::Matrix4d matrixViewTranslate;
	Eigen::Matrix4d matrixModel;
	Eigen::Matrix4d matrixModelView;
	Eigen::Matrix4d matrixProjection;


	int mouseX; int mouseY;
	double cameraDistance;

};