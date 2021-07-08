
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

        proType = ORTHOGRAPHIC;
        fovy = 45.0; aspect = 1.3; // approximate 800/600=640/480
        z_near = 0.1; z_far = 100.0;
        x_left = y_bottom = -100.0; x_right = y_top = 100.0;

    }

    Viewer::~Viewer()
    {

    }

    void Viewer::PrintViewInfo()
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

        ExtractUVWE();
        ComposeViewMatrix();
    }

    void Viewer::SetViewTranslateMatrix(const Mat4 &_m)
    {
        view_translate_matrix = _m;

        ExtractUVWE();
        ComposeViewMatrix();
    }

    void Viewer::SetViewRotateMatrix(const Mat4 &_m)
    {
        assert(IsRotationMatrix(_m));
        view_rotate_matrix = _m;

        ExtractUVWE();
        ComposeViewMatrix();
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

        ExtractUVWE();
        ComposeViewMatrix();
    }

    void Viewer::SetViewMatrix(const Mat4 &_m)
    {
        assert(IsRotationMatrix(_m));
        Mat3 tmp_rotate = _m; // implicit convert Mat4 to Mat3
        view_rotate_matrix = tmp_rotate; // implicit convert Mat3 to Mat4
        view_translate_matrix[3] = transpose(view_rotate_matrix) * _m[3];

        ExtractUVWE();
        ComposeViewMatrix();
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
        ComposeViewMatrix();
    }

    void Viewer::ExtractUVWE()
    {
        u.x = view_rotate_matrix[0][0]; u.y = view_rotate_matrix[1][0]; u.z = view_rotate_matrix[2][0];
        v.x = view_rotate_matrix[0][1]; v.y = view_rotate_matrix[1][1]; v.z = view_rotate_matrix[2][1];
        w.x = view_rotate_matrix[0][2]; w.y = view_rotate_matrix[1][2]; w.z = view_rotate_matrix[2][2];
        e.x = -view_translate_matrix[3][0]; e.y = -view_translate_matrix[3][1]; e.z = -view_translate_matrix[3][2];
    }

    void Viewer::ComposeViewMatrix()
    {
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

    void Viewer::Pitch(scalar _radians)
    {
        /*
        $$\mathbf{R}_{x}(\alpha)=\left(\begin{array}{cccc}
            1 & 0 & 0 & 0 \\
            0 & \cos \alpha & -\sin \alpha & 0 \\
            0 & \sin \alpha & \cos \alpha & 0 \\
            0 & 0 & 0 & 1
            \end{array}\right)
        $$
        */
        Mat4 rotation(1.0);
        // [2][1] 1-st row, 2-st column
        rotation[1][1] = rotation[2][2] = std::cos(_radians);
        rotation[1][2] = std::sin(_radians);
        rotation[2][1] = -rotation[1][2];
        view_rotate_matrix = rotation * view_rotate_matrix;
        ExtractUVWE();
        ComposeViewMatrix();
    }

    void Viewer::Yaw(scalar _radians)
    {
        /*
        $$\mathbf{R}_{y}(\alpha)=\left(\begin{array}{cccc}
            \cos \alpha & 0 & \sin \alpha & 0 \\
            0 & 1 & 0 & 0 \\
            -\sin \alpha & 0 & \cos \alpha & 0 \\
            0 & 0 & 0 & 1
            \end{array}\right)
        $$
        */
        Mat4 rotation(1.0);
        rotation[0][0] = rotation[2][2] = std::cos(_radians);
        rotation[2][0] = std::sin(_radians);
        rotation[0][2] = -rotation[2][0];
        view_rotate_matrix = rotation * view_rotate_matrix;
        ExtractUVWE();
        ComposeViewMatrix();
    }

    void Viewer::Roll(scalar _radians)
    {
        /*
        $$\mathbf{R}_{z}(\alpha)=\left(\begin{array}{cccc}
            \cos \alpha & -\sin \alpha & 0 & 0 \\
            \sin \alpha & \cos \alpha & 0 & 0 \\
            0 & 0 & 1 & 0 \\
            0 & 0 & 0 & 1
            \end{array}\right)
        $$
        */
        Mat4 rotation(1.0);
        rotation[0][0] = rotation[1][1] = std::cos(_radians);
        rotation[0][1] = std::sin(_radians);
        rotation[1][0] = -rotation[0][1];
        view_rotate_matrix = rotation * view_rotate_matrix;
        ExtractUVWE();
        ComposeViewMatrix();
    }

    bool Viewer::IsValidProjection()
    {
        bool ret = false;
        if (z_near > 0 || z_far > 0)
        {
            ret = true;
        }

        return ret;
    }

    void Viewer::Orthographic(scalar _left, scalar _right, scalar _bottom, scalar _top, scalar _near, scalar _far)
    {
        // this implementation has 1e-6 precision in last column elements
        // compare with glm::ortho because of matirx multiplation
        // the better implementation is to construct one matrix with symbol solution
        proType = ORTHOGRAPHIC;

        z_near = -_near; z_far = -_far;
        assert(IsValidProjection());
        x_left = _left; x_right = _right; y_top = _top; y_bottom = _bottom;

        Mat4 trans(1.0);
        trans[3] = Vec4(-(x_left + x_right) / 2.0, -(y_top + y_bottom) / 2.0, \
                        -(z_near + z_far) / 2.0, 1.0);

        Mat4 scl(1.0);
        scl[0][0] = 2.0 / (x_right - x_left);
        scl[1][1] = 2.0 / (y_top - y_bottom);
        // result in NDC space is left-hand, beacause z is negative
        // as shown in http://www.songho.ca/opengl/files/gl_projectionmatrix01.png
        scl[2][2] = 2.0 / (z_far - z_near);

        projection_matrix = scl * trans;
    }

    void Viewer::Perspective(scalar _left, scalar _right, scalar _bottom, scalar _top, scalar _near, scalar _far)
    {

    }















}
