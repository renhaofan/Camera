/*****************************************************************
   Implement viewMatrix with transformation for OpenGL to utilize
*****************************************************************/

/*****************************************************************

         z|                                w|
    x     |          M_{view}        u      |
    ------0       ------------>      -------e
           \								 \
            \y								  \v
Note: gazing direction is -w, not w, u is up-direction
*****************************************************************/

#ifndef Viewer_H
#define Viewer_H

#include <iostream>
#include <limits>
#include "../Geometry/Geometry.h"


namespace viewer {
    // setup cout precision
    typedef std::numeric_limits<float> dfl;

    class Viewer
    {
    public:
        Viewer();
        ~Viewer();
        // Suppress the default copy constructor and assignment operator
        Viewer(const Viewer&);
        Viewer& operator=(const Viewer&);

        void PrintInfo();

        // setters
//        void SetViewTranslateMatrix(const Eigen::Vector3f& m);
//        void SetViewTranslateMatrix(const Eigen::Matrix4f& m);
//        void SetViewTranslateMatrix(float tx, float ty, float tz) {
//            SetViewTranslateMatrix(Eigen::Vector3f(tx, ty, tz));
//        }

//        void SetViewRotateMatrix(const Eigen::Matrix3f& m);
//        void SetViewRotateMatrix(const Eigen::Matrix4f& m);
//        void SetViewRotateMatrix(const Eigen::Vector3f& u, const Eigen::Vector3f& v, const Eigen::Vector3f& w);

//        void SetViewMatrix(const Eigen::Matrix4f& m);
//        void SetViewMatrix(const Eigen::Vector3f& u, const Eigen::Vector3f& v, const Eigen::Vector3f& w, const Eigen::Vector3f& e);



//        void SetCamera(const Eigen::Vector3f& pos, const Eigen::Vector3f& tar);
//        void SetCamera(const Eigen::Vector3f& pos, const Eigen::Vector3f& tar, const Eigen::Vector3f& upDir);
//        void SetCamera(float px, float py, float pz, float tx, float ty, float tz) {
//            SetCamera(Eigen::Vector3f(px, py, pz), Eigen::Vector3f(tx, ty, tz));
//        }
//        void SetCamera(float px, float py, float pz, float tx, float ty, float tz, float ux, float uy, float uz) {
//            SetCamera(Eigen::Vector3f(px, py, pz), Eigen::Vector3f(tx, ty, tz), Eigen::Vector3f(ux, uy, uz));
//        }
//        // similar to gluLookAt()
//        void LookAt(float px, float py, float pz, float tx, float ty, float tz, float ux, float uy, float uz) {
//            SetCamera(px, py, pz, tx, ty, tz, ux, uy, uz);





    private:
        glm::vec3 u, v, w, e;

        // row-major
        glm::mat4* view_matrix;
        glm::mat4* view_rotate_matrix;
        glm::mat4* view_translate_matrix;

        glm::mat4* projection_matrix;

    };

}




#endif // Viewer_H
