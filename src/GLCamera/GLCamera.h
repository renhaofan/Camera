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
        ~Viewer();
        // Suppress the default copy constructor and assignment operator
        Viewer(const Viewer&);
        Viewer& operator=(const Viewer&);

        void PrintInfo();
        void PrintMat4(const Mat4& m);
        /*
         *  NOTES:  Mat4 can be passed in, beacuse of implicit type convertion
         *  Only judeg Mat.block<3, 3>(0, 0) whether is rotaion matrix
         *  Needed to add more constraint
        */
        bool IsRotationMatrix(const Mat3& m);


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
        void ShiftLeft(scalar delta_left, scalar speed = 1.0);
        void ShiftRight(scalar delta_right, scalar speed = 1.0);
        void ShiftUp(scalar delta_up, scalar speed = 1.0);
        void ShiftDown(scalar delta_down, scalar speed = 1.0);
        void ShiftForward(scalar delta_forward, scalar speed = 1.0);
        void ShiftBackward(scalar delta_backward, scalar speed = 1.0);

        // angle in radians by default
        void Pitch(scalar _radians); // around u
        void Yaw(scalar _radians);  // aroud v
        void Roll(scalar _radians); // aroud w

    private:

        Vec3 u, v, w, e;
        Mat4 view_matrix;
        Mat4 view_rotate_matrix;
        Mat4 view_translate_matrix;


    };



}




#endif // Viewer_H
