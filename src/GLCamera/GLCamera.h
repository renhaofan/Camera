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
        bool IsRotationMatrix(const Mat3& m);
        bool IsRotationMatrix(const Mat4& m)
        {
            // explict convert Mat4 to Mat3
            return IsRotationMatrix(m);
        }



        // setters
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

        //    extract uvwe     from translation and roataion matrix
        // compose view matrix with translation and roataion matrix
        void UpdateViewMatrix();

        void LookAt(const Vec3& _pos, const Vec3& _tar, const Vec3& upDir);
        void LookAt(scalar _px, scalar _py, scalar _pz, scalar _tx, scalar _ty, scalar _tz, scalar _ux, scalar _uy, scalar _uz)
        {
            SetCamera(Vec3(_px, _py, _pz), Vec3(_tx, _ty, _tz), Vec3(ux, _uy, _uz));
        }



    private:

        Vec3 u, v, w, e;
        Mat4 view_matrix;
        Mat4 view_rotate_matrix;
        Mat4 view_translate_matrix;


    };



}




#endif // Viewer_H
