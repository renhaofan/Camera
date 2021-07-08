
#include "GLCamera.h"

namespace viewer
{
    Viewer::Viewer()
    {
        u = Vec3(1.0, 0.0, 0.0);
        v = Vec3(0.0, 1.0, 0.0);
        w = Vec3(0.0, 0.0, 1.0);
        e = Vec3(0.0, 0.0, -10.0);
        view_matrix = Mat4(1.0);
        view_rotate_matrix = Mat4(1.0);
        view_translate_matrix = Mat4(1.0);

//        projection_matrix = mmat4(1.0);
//        eye_fov = 45.0;
////        aspect_ratio =
//        zNear = 0.1;
        //        zFar = 100.0;
    }

    Viewer::~Viewer()
    {

    }

    void Viewer::PrintInfo()
    {
        // make sure conversion of a floating-point value to text and back is exact using max_digits10
        std::cout.precision(dfl::max_digits10);
        std::cout << "Camera Position: \n";
        std::cout << "e: " << e.x << " " << e.y << " " << e.z << "\n";
        std::cout << "u: " << u.x << " " << u.y << " " << u.z << "\n";
        std::cout << "v: " << v.x << " " << v.y << " " << v.z << "\n";
        std::cout << "w: " << w.x << " " << w.y << " " << w.z << "\n";
        std::cout << "view_rotate_matrix:\n"; PrintMat4(view_rotate_matrix);
        std::cout << "view_translate_matrix:\n";PrintMat4(view_translate_matrix);
        std::cout << "view_matrix:\n";PrintMat4(view_matrix);
    }

    void Viewer::PrintMat4(const Mat4& m)
    {
        std::cout << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n";
        std::cout << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n";
        std::cout << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n";
        std::cout << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << "\n";
    }

    bool Viewer::IsRotationMatrix(const Mat3 &m)
    {
        bool isRotation = false;
        if (determinant(m) - 1.0 < 1e-6)
        {
            isRotation = true;
        }
        return isRotation;
    }

    void Viewer::SetViewTranslateMatrix(const Vec3 &_v)
    {
//        view_translate_matrix[3][0] = _v.x;
//        view_translate_matrix[3][1] = _v.y;
//        view_translate_matrix[3][2] = _v.z;
        view_translate_matrix[3] = Vec4(_v.x, _v.y, _v.z, 1.0);
        UpdateViewMatrix();
    }

    void Viewer::SetViewTranslateMatrix(const Mat4 &_m)
    {
        view_translate_matrix = _m;
        UpdateViewMatrix();
    }

    void Viewer::SetViewRotateMatrix(const Mat4 &_m)
    {
        assert(IsRotationMatrix(_m));
        view_rotate_matrix = _m;
        UpdateViewMatrix();
    }

    void Viewer::SetViewRotateMatrix(const Vec3 &_u, const Vec3 &_v, const Vec3 &_w)
    {
        Vec3 tmp_u = normalize(_u);
        Vec3 tmp_v = normalize(_v);
        Vec3 tmp_w = normalize(_w);
        view_rotate_matrix[0] = Vec4(tmp_u.x, tmp_v.x, tmp_w.x, 0.0);
        view_rotate_matrix[1] = Vec4(tmp_u.y, tmp_v.y, tmp_w.y, 0.0);
        view_rotate_matrix[2] = Vec4(tmp_u.z, tmp_v.z, tmp_w.z, 0.0);
        assert(IsRotationMatrix(view_rotate_matrix));
        UpdateViewMatrix();
    }

    void Viewer::SetViewMatrix(const Mat4 &_m)
    {
        assert(IsRotationMatrix(_m));
        Mat3 tmp_rotate = _m; // implicit convert Mat4 to Mat3
        view_rotate_matrix = tmp_rotate; // implicit convert Mat3 to Mat4
        view_translate_matrix[3] = transpose(view_rotate_matrix) * _m[3];
        UpdateViewMatrix();
    }

    // compose view matrix from u, v, w, e
    void Viewer::SetViewMatrix(const Vec3& _u, const Vec3& _v, const Vec3& _w, const Vec3& _e)
    {
        u = _u; v= _v; w = _w; e = _e;
        view_translate_matrix[3] = Vec4(-e.x, -e.y, -e.z, 1.0);
        Mat4 tmp_rotate = Mat4(1.0);
        tmp_rotate[0] = Vec4(u.x, u.y, u.z, 0.0);
        tmp_rotate[1] = Vec4(v.x, v.y, v.z, 0.0);
        tmp_rotate[2] = Vec4(w.x, w.y, w.z, 0.0);
        assert(IsRotationMatrix(tmp_rotate));
        view_rotate_matrix = transpose(tmp_rotate);
        view_matrix = view_rotate_matrix * view_translate_matrix;
    }


    void Viewer::UpdateViewMatrix()
    {
//        e = Vec3(view_translate_matrix[3][0], view_translate_matrix[3][0], view_translate_matrix[3][0]);
//        u = Vec3(view_translate_matrix[0][0], view_translate_matrix[1][0], view_translate_matrix[2][0]);
//        v = Vec3(view_translate_matrix[0][1], view_translate_matrix[1][1], view_translate_matrix[2][1]);
//        w = Vec3(view_translate_matrix[0][2], view_translate_matrix[1][2], view_translate_matrix[2][2]);
          u.x = view_rotate_matrix[0][0]; u.y = view_rotate_matrix[1][0]; u.z = view_rotate_matrix[2][0];
          v.x = view_rotate_matrix[0][1]; v.y = view_rotate_matrix[1][1]; v.z = view_rotate_matrix[2][1];
          w.x = view_rotate_matrix[0][2]; w.y = view_rotate_matrix[1][2]; w.z = view_rotate_matrix[2][2];
          e.x = -view_translate_matrix[3][0]; e.y = -view_translate_matrix[3][1]; e.z = -view_translate_matrix[3][2];
          view_matrix = view_rotate_matrix * view_translate_matrix;
    }

    void Viewer::LookAt(const Vec3 &_pos, const Vec3 &_tar, const Vec3 &_up)
    {
        Vec3 tmp_u, tmp_v, tmp_w;
        tmp_w = normalize(_tar - _pos);
        tmp_v = normalize(_up);
        tmp_u = cross(tmp_w, tmp_v);
        tmp_v = cross(tmp_u, tmp_w);
        tmp_w = -tmp_w;
        SetViewMatrix(tmp_u, tmp_v, tmp_w, _pos);
    }

    void Viewer::ShiftLeft(scalar delta_left, scalar speed)
    {
        if (delta_left <= 0)
        {
            ShiftRight(-delta_left, speed);
        } else
        {
            SetViewTranslateMatrix(delta_left * speed * u - e);
        }
    }

    void Viewer::ShiftRight(scalar delta_right, scalar speed)
    {
        if (delta_right <= 0)
        {
            ShiftRight(-delta_right, speed);
        } else
        {
            SetViewTranslateMatrix(-delta_right * speed * u - e);
        }
    }

    void Viewer::ShiftUp(scalar delta_up, scalar speed)
    {
        if (delta_up <= 0)
        {
            ShiftRight(-delta_up, speed);
        } else
        {
            SetViewTranslateMatrix(-delta_up * speed * v - e);
        }
    }

    void Viewer::ShiftDown(scalar delta_down, scalar speed)
    {
        if (delta_down <= 0)
        {
            ShiftRight(-delta_down, speed);
        } else
        {
            SetViewTranslateMatrix(delta_down * speed * v - e);
        }
    }

    void Viewer::ShiftForward(scalar delta_forward, scalar speed)
    {
        if (delta_forward <= 0)
        {
            ShiftRight(-delta_forward, speed);
        } else
        {
            SetViewTranslateMatrix(delta_forward * speed * w - e);
        }
    }

    void Viewer::ShiftBackward(scalar delta_backward, scalar speed)
    {
        if (delta_backward <= 0)
        {
            ShiftRight(-delta_backward, speed);
        } else
        {
            SetViewTranslateMatrix(-delta_backward * speed * w - e);
        }
    }















}
