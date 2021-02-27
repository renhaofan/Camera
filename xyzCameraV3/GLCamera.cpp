
#include "GLCamera.hpp"

const double DEG2RAD = 3.141593f / 180.0f;
const double RAD2DEG = 180.0f / 3.141593f;
const double EPSILON = 0.00001f;

GLCamera::GLCamera() {

	this->matrixViewRotate.setIdentity();
	this->matrixViewTranslate.setIdentity();
	this->matrixView = matrixViewRotate * matrixViewTranslate;

	this->matrixModel.setIdentity();
	this->matrixModelView = matrixView * matrixModel;
	this->matrixProjection.setIdentity();



	this->mouseX = this->mouseY = 0;
	this->cameraDistance = 0;
	this->cameraPosition = Eigen::Vector3d(0, 0, 0);
	this->target = Eigen::Vector3d(0, 0, 0); // this means target, not the real focal;
	this->distance = (target - cameraPosition).norm();
	this->cameraAngle = Eigen::Vector3d(0, 0, 0);

	// setModelViewMatrix();
	//this->intrinsics.setIdentity();
//	this->extrinsics.setIdentity();

}

GLCamera::~GLCamera() {

}

///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////
void GLCamera::init() {
	// glEnable(GL_DEPTH_TEST);
	//glClearColor(1.0, 1.0, 1.0, 0.0);//设置清除颜色
	//glClear(GL_COLOR_BUFFER_BIT);//把窗口清除为当前颜色

}

///////////////////////////////////////////////////////////////////////////////
// clean up OpenGL objects
///////////////////////////////////////////////////////////////////////////////
void GLCamera::quit() {

}

void GLCamera::printSelf() {
	std::cout << "====GLCamera======\n"
		<< "  Position.T(): " << cameraPosition.transpose() << "\n"
		<< "    Target.T():   " << target.transpose() << "\n"
		<< "    matrixModelView:     \n" << matrixModelView << "\n"
		<< "  Distance: " << this->distance << std::endl;
}

void GLCamera::zoomCameraDelta(double delta) {
	if (delta > 0) this->shiftForward(delta);
	else this->shiftBackward(-delta);
}


#pragma region(shift)
void GLCamera::shiftTo(const Eigen::Vector3d& to) {
	Eigen::Vector3d forward = to - cameraPosition;
	Eigen::Vector3d newTarget = target + forward;


	this->cameraPosition = to;
	this->target = newTarget;

	updateDistance();

	this->matrixViewTranslate.block<3, 1>(0, 3) = -to;
	updateViewMatrix();
	updateModelViewMatrix();
}


// delta > 0, right, up, backward;
void GLCamera::shift(const Eigen::Vector3d& delta) {

	Eigen::Matrix4d m = matrixViewRotate.transpose();


	Eigen::Vector3d deltaRight = delta(0) * m.block<3, 1>(0, 0);
	Eigen::Vector3d deltaUp = delta(1) * m.block<3, 1>(0, 1);
	Eigen::Vector3d deltaBackward = delta(2) * m.block<3, 1>(0, 2);

	Eigen::Vector3d newPosition = cameraPosition + deltaRight + deltaUp + deltaBackward;

	shiftTo(newPosition);
}



void GLCamera::shiftLeft(double deltaLeft) {
	if (deltaLeft <= 0) {
		shiftRight(-deltaLeft);
	}
	else {
		shift(-deltaLeft, 0, 0);
	}
};

void GLCamera::shiftRight(double deltaRight) {
	if (deltaRight <= 0) {
		shiftLeft(-deltaRight);
	}
	else {
		shift(deltaRight, 0, 0);
	}
};

void GLCamera::shiftUp(double deltaUp) {
	if (deltaUp <= 0) {
		shiftDown(-deltaUp);
	}
	else {
		shift(0, deltaUp, 0);
	}
};

void GLCamera::shiftDown(double deltaDown) {
	if (deltaDown <= 0) {
		shiftUp(-deltaDown);
	}
	else {
		shift(0, -deltaDown, 0);
	}
};

void GLCamera::shiftForward(double deltaForward) {
	if (deltaForward <= 0) {
		shiftBackward(-deltaForward);
	}
	else {
		shift(0, 0, -deltaForward);
	}
};

void GLCamera::shiftBackward(double deltaBackward) {
	if (deltaBackward <= 0) {
		shiftForward(-deltaBackward);
	}
	else {
		shift(0, 0, deltaBackward);
	}
};


#pragma endregion(shift)

#pragma region(rotate)
void GLCamera::pitch(double degree) {
	Eigen::Matrix4d rotate;
	rotate.setIdentity();
	rotate(1, 1) = std::cos(degree * DEG2RAD);
	rotate(2, 2) = std::cos(degree * DEG2RAD);
	rotate(1, 2) = -std::sin(degree * DEG2RAD);
	rotate(2, 1) = std::sin(degree * DEG2RAD);

	matrixViewRotate *= rotate;
	updateViewMatrix();
	updateModelViewMatrix();

}
void GLCamera::yaw(double degree) {
	Eigen::Matrix4d rotate;
	degree = -degree;
	rotate.setIdentity();
	rotate(0, 0) = std::cos(degree * DEG2RAD);
	rotate(2, 2) = std::cos(degree * DEG2RAD);
	rotate(2, 0) = -std::sin(degree * DEG2RAD);
	rotate(0, 2) = std::sin(degree * DEG2RAD);

	matrixViewRotate *= rotate;


	updateViewMatrix();
	updateModelViewMatrix();
}
void GLCamera::roll(double degree) {
	Eigen::Matrix4d rotate;
	rotate.setIdentity();
	rotate(0, 0) = std::cos(degree * DEG2RAD);
	rotate(1, 1) = std::cos(degree * DEG2RAD);
	rotate(1, 0) = std::sin(degree * DEG2RAD);
	rotate(0, 1) = -std::sin(degree * DEG2RAD);

	matrixViewRotate *= rotate;
	updateViewMatrix();
	updateModelViewMatrix();
}

#pragma endregion(rotate)

#pragma region(setCamera)
void GLCamera::setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar) {
	this->cameraPosition = pos;
	this->target = tar;
	updateDistance();

	this->matrixViewTranslate.block<3, 1>(0, 3) = -pos;



	Eigen::Vector3d w = pos - tar; w.normalize();
	Eigen::Vector3d v;


	if (abs(this->cameraPosition.x()) < EPSILON && abs(this->cameraPosition.z()) < EPSILON) {
		if (w.y() > 0) {
			v = Eigen::Vector3d(0, 0, -1);
		}
		else {
			v = Eigen::Vector3d(0, 0, 1);
		}
	}
	else
	{
		v = Eigen::Vector3d(0, 1, 0);
	}

	Eigen::Vector3d u = v.cross(w); u.normalize();
	// recompute up vector, make three axis orthometric
	v = w.cross(u); v.normalize();

	Eigen::Matrix4d uvw; uvw.setIdentity();
	uvw.block<3, 1>(0, 0) = u;
	uvw.block<3, 1>(0, 1) = v;
	uvw.block<3, 1>(0, 2) = w;



	this->matrixViewRotate = uvw.transpose();
	updateViewMatrix();
	updateModelViewMatrix();
}


void GLCamera::setCamera(Eigen::Vector3d pos, Eigen::Vector3d tar, Eigen::Vector3d upDir) {
	this->cameraPosition = pos;
	this->target = tar;
	updateDistance();

	this->matrixViewTranslate.block<3, 1>(0, 3) = -pos;



	Eigen::Vector3d w = pos - tar; w.normalize();
	Eigen::Vector3d v = upDir;


	Eigen::Vector3d u = v.cross(w); u.normalize();
	// recompute up vector, make three axis orthometric
	v = w.cross(u); v.normalize();

	Eigen::Matrix4d uvw; uvw.setIdentity();
	uvw.block<3, 1>(0, 0) = u;
	uvw.block<3, 1>(0, 1) = v;
	uvw.block<3, 1>(0, 2) = w;



	this->matrixViewRotate = uvw.transpose();
	updateViewMatrix();
	updateModelViewMatrix();
}
#pragma endregion(setCamera)
