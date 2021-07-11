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
#include <cmath>

#include "../Geometry/Geometry.h"

namespace viewer
{
    // setup cout precision
    using namespace geometry;

    class Viewer
    {
    public:
        Viewer();
        ~Viewer() = default;
        // Suppress the default copy constructor and assignment operator
        Viewer(const Viewer&);
        Viewer& operator=(const Viewer&);

        void PrintMat4(const Mat4& m);





        /**********************************************************************
         * setup viewmatrix
         * translation function u,v,w
         * rotation function u,v,w
        ***********************************************************************/

        /*
         *  NOTES:  Mat4 can be passed in, beacuse of implicit type convertion
         *  Only judeg Mat.block<3, 3>(0, 0) whether is rotaion matrix
         *  Needed to add more constraint
        */
        bool IsRotationMatrix(const Mat3& m);
        void PrintViewInfo();

        // setters
        // assign values directly without any transformation
        void SetViewTranslateMatrix(const Vec3& _v);
        void SetViewTranslateMatrix(const Mat4& _m);
        void SetViewTranslateMatrix(scalar tx, scalar ty, scalar tz)
        {
            SetViewTranslateMatrix(Vec3(tx, ty, tz));
        }

        void SetViewRotateMatrix(const Mat4& _m);
        void SetViewRotateMatrix(const Vec3& _u, const Vec3& _v, const Vec3& _w);
        void SetViewRotateMatrix(const Mat3& _m)
        {
            SetViewRotateMatrix(Mat4(_m));
        }

        void SetViewMatrix(const Mat4& _m);
        void SetViewMatrix(const Vec3& _u, const Vec3& _v, const Vec3& _w, const Vec3& _e);

        void setViewPosition(const Vec3& _e)
        {
            SetViewTranslateMatrix(-_e);
        }
        void setViewPosition(scalar _x, scalar _y, scalar _z)
        {
            SetViewTranslateMatrix(Vec3(_x, _y, _z));
        }

    private:
        //    extract uvwe     from translation and roataion matrix
        void ExtractUVWE();
        // compose view matrix with translation and roataion matrix
        void ComposeViewMatrix();

    public:
        void LookAt(const Vec3& _pos, const Vec3& _tar, const Vec3& upDir);
        void LookAt(scalar _px, scalar _py, scalar _pz, scalar _tx, scalar _ty, scalar _tz, scalar _ux, scalar _uy, scalar _uz)
        {
            LookAt(Vec3(_px, _py, _pz), Vec3(_tx, _ty, _tz), Vec3(_ux, _uy, _uz));
        }

        // getter
        Mat4 GetViewMatirx() const { return view_matrix; }
        Vec3 GetPosition() const { return e; }

        // delta > 0, shift direction---camera coordinates: right(u), up(v), backward(w);
        void ShiftLeft(scalar delta_left, scalar sensitivity = 1.0);
        void ShiftRight(scalar delta_right, scalar sensitivity = 1.0);
        void ShiftUp(scalar delta_up, scalar sensitivity = 1.0);
        void ShiftDown(scalar delta_down, scalar sensitivity = 1.0);
        void ShiftForward(scalar delta_forward, scalar sensitivity = 1.0);
        void ShiftBackward(scalar delta_backward, scalar sensitivity = 1.0);

        // angle(delta value) in radians by default,
        void Pitch(scalar _radians, scalar _sensitivity = 1.0); // around u
        void Yaw(scalar _radians, scalar _sensitivity = 1.0);  // aroud v
        void Roll(scalar _radians, scalar _sensitivity = 1.0); // aroud w

        /**********************************************************************
         *              setup projection matrix
         *      Both z_near and z_far are negative
         *
        ***********************************************************************/

        enum PType
        {
            ORTHOGRAPHIC,
            PERSPECTIVE
        };

        // check whether fovy, aspect, zNear, zFar xLeft, xRight ...is valid value;
        bool IsPositiveBoth(scalar _z_near, scalar _z_far);
        bool IsValidFovyRatio(scalar _fovy, scalar _ratio);

        void PrintProjectionMatrix();
        // projection
        // args near, far > 0, so that member variable z_near, z_far  < 0
        void Orthographic(scalar _left, scalar _right, scalar _bottom, scalar _top, scalar _near, scalar _far);

        void frustum(scalar _left, scalar _right, scalar _bottom, scalar _top, scalar _near, scalar _far);
        void Perspective(scalar _fovy, scalar _aspect, scalar _zNear, scalar _zFar);

        void Perspective(scalar _fovy, scalar _width, scalar _height, scalar _zNear, scalar _zFar)
        {
            Perspective(_fovy, _width/_height, _zNear, _zFar);
        }

        Mat4 GetProjectionMatrix() { return projection_matrix; }

    private:

        Vec3 u, v, w, e;
        Mat4 view_matrix;
        Mat4 view_rotate_matrix;
        Mat4 view_translate_matrix;

        //aspect SCR_WIDTH / (float)SCR_HEIGHT
        PType proType;
        scalar fovy, aspect, z_near, z_far;
        scalar x_left, x_right, y_top, y_bottom;
        Mat4 projection_matrix;
    };



}




#endif // Viewer_H
