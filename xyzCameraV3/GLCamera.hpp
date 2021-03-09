#pragma once


//¡¡matrix store element is row-major

/*****************************************************************
   Implement viewMatrix with transformation for OpenGL to utilize
*****************************************************************/

/*****************************************************************
  
         z|                                w|
	x     |          M_{view}        u      |
    ------0       ------------>      -------e 
		   \								 \
		    \y								  \v
Note: gazing direction is -w, not w
*****************************************************************/

#define DEG2RAD (3.1415926535f / 180.0f)
#define RAD2DEG (180.0f / 3.1415926535f)

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>


class GLCamera {
public:
	GLCamera();
	~GLCamera();
	// print info
	void PrintInfo();
	// setters
	void SetViewTranslateMatrix(const Eigen::Vector3d& m);
	void SetViewTranslateMatrix(const Eigen::Matrix4d& m);
	void SetViewTranslateMatrix(double tx, double ty, double tz) {
		SetViewTranslateMatrix(Eigen::Vector3d(tx, ty, tz));
	}

	void SetViewRotateMatrix(const Eigen::Matrix3d& m);
	void SetViewRotateMatrix(const Eigen::Matrix4d& m);
	void SetViewRotateMatrix(const Eigen::Vector3d& u, const Eigen::Vector3d& v, const Eigen::Vector3d& w); 
	
	void SetViewMatrix(const Eigen::Matrix4d& m);
	void SetViewMatrix(const Eigen::Vector3d& u, const Eigen::Vector3d& v, const Eigen::Vector3d& w, const Eigen::Vector3d& e);



	void SetCamera(const Eigen::Vector3d& pos, const Eigen::Vector3d& tar);
	void SetCamera(const Eigen::Vector3d& pos, const Eigen::Vector3d& tar, const Eigen::Vector3d& upDir);
	void SetCamera(double px, double py, double pz, double tx, double ty, double tz) {
		SetCamera(Eigen::Vector3d(px, py, pz), Eigen::Vector3d(tx, ty, tz));
	}
	void SetCamera(double px, double py, double pz, double tx, double ty, double tz, double ux, double uy, double uz) {
		SetCamera(Eigen::Vector3d(px, py, pz), Eigen::Vector3d(tx, ty, tz), Eigen::Vector3d(ux, uy, uz));
	}
    // similar to gluLookAt()
	void LookAt(double px, double py, double pz, double tx, double ty, double tz, double ux, double uy, double uz) {
		SetCamera(px, py, pz, tx, ty, tz, ux, uy, uz);
	}


	// update
	void UpdateViewMatrix() {
		_u = _view_rotate_matrix.block<1, 3>(0, 0);
		_v = _view_rotate_matrix.block<1, 3>(1, 0);
		_w = _view_rotate_matrix.block<1, 3>(2, 0);
		_e = _view_translate_matrix.block<3, 1>(0, 3);
		_e = -_e;
		_view_matrix = _view_rotate_matrix * _view_translate_matrix;
	}


	//getters
	Eigen::Matrix4d GetViewMatrix() {
		return _view_matrix;
	}
	Eigen::Vector3d GetViewTranslate() {
		return _view_translate_matrix.block<3, 1>(0, 3);
	}

	// translate to position t
	void TranslateTo(const Eigen::Vector3d& t) {
		SetViewTranslateMatrix((Eigen::Vector3d)(-t));
	}
	void TranslateTo(double tx, double ty, double tz) {
		SetViewTranslateMatrix(-tx, -ty, -tz);
	}

	// delta > 0, shift direction---right(u), up(v), backward(w);
	void shiftLeft(double deltaLeft);
	void shiftRight(double deltaRight);
	void shiftUp(double deltaUp);
	void shiftDown(double deltaDown);
	void shiftForward(double deltaForward);
	void shiftBackward(double deltaBackward);



	// to vector homogeneous coordinates
	Eigen::Vector4d ToHomogeneous(const Eigen::Vector3d& v) {
		return Eigen::Vector4d(v.x(), v.y(), v.z(), 0);
	}
	// to vector nonhomogeneous coordinates
	Eigen::Vector3d ToNonhomogeneous(const Eigen::Vector4d& v) {
		if (std::abs(v[3]) < 1e-5) {
			return Eigen::Vector3d(v.x(), v.y(), v.z());
		}
		else {
			return Eigen::Vector3d(v.x()/v.w(), v.y()/v.w(), v.z()/v.w());
		}
	}


protected:

private:
	// Camera e is camera 
	Eigen::Vector3d _u, _v, _w, _e;		   // right-hand corresponding to x,y,z,orign(0, 0, 0)
	
	// 4x4 transform matrices
	Eigen::Matrix4d _view_matrix;
	Eigen::Matrix4d _view_rotate_matrix; // x,y,z  row-major
	Eigen::Matrix4d _view_translate_matrix;
};