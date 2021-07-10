
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

        proType = PERSPECTIVE;
        fovy = 45.0; aspect = 1.3; // approximate 800/600=640/480
        z_near = 0.1; z_far = 100.0;
        Perspective(fovy, aspect, z_near, z_far);
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

    void Viewer::ShiftLeft(scalar delta_left, scalar sensitivity)
    {
        if (delta_left <= 0)
        {
            ShiftRight(-delta_left, sensitivity);
        } else
        {
            SetViewTranslateMatrix(delta_left * sensitivity * u - e);
        }
    }

    void Viewer::ShiftRight(scalar delta_right, scalar sensitivity)
    {
        if (delta_right <= 0)
        {
            ShiftRight(-delta_right, sensitivity);
        } else
        {
            SetViewTranslateMatrix(-delta_right * sensitivity * u - e);
        }
    }

    void Viewer::ShiftUp(scalar delta_up, scalar sensitivity)
    {
        if (delta_up <= 0)
        {
            ShiftRight(-delta_up, sensitivity);
        } else
        {
            SetViewTranslateMatrix(-delta_up * sensitivity * v - e);
        }
    }

    void Viewer::ShiftDown(scalar delta_down, scalar sensitivity)
    {
        if (delta_down <= 0)
        {
            ShiftRight(-delta_down, sensitivity);
        } else
        {
            SetViewTranslateMatrix(delta_down * sensitivity * v - e);
        }
    }

    void Viewer::ShiftForward(scalar delta_forward, scalar sensitivity)
    {
        if (delta_forward <= 0)
        {
            ShiftRight(-delta_forward, sensitivity);
        } else
        {
            SetViewTranslateMatrix(delta_forward * sensitivity * w - e);
        }
    }

    void Viewer::ShiftBackward(scalar delta_backward, scalar sensitivity)
    {
        if (delta_backward <= 0)
        {
            ShiftRight(-delta_backward, sensitivity);
        } else
        {
            SetViewTranslateMatrix(-delta_backward * sensitivity * w - e);
        }
    }

    void Viewer::Pitch(scalar _radians, scalar _sensitivity)
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
        scalar tmp_radians = _radians * _sensitivity;
        Mat4 rotation(1.0);
        // [2][1] 1-st row, 2-st column
        rotation[1][1] = rotation[2][2] = std::cos(tmp_radians);
        rotation[1][2] = std::sin(tmp_radians);
        rotation[2][1] = -rotation[1][2];
        view_rotate_matrix = rotation * view_rotate_matrix;
        ExtractUVWE();
        ComposeViewMatrix();
    }

    void Viewer::Yaw(scalar _radians, scalar _sensitivity)
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
        scalar tmp_radians = _radians * _sensitivity;
        Mat4 rotation(1.0);
        rotation[0][0] = rotation[2][2] = std::cos(tmp_radians);
        rotation[2][0] = std::sin(tmp_radians);
        rotation[0][2] = -rotation[2][0];
        view_rotate_matrix = rotation * view_rotate_matrix;
        ExtractUVWE();
        ComposeViewMatrix();
    }

    void Viewer::Roll(scalar _radians, scalar _sensitivity)
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
        scalar tmp_radians = _radians * _sensitivity;
        Mat4 rotation(1.0);
        rotation[0][0] = rotation[1][1] = std::cos(tmp_radians);
        rotation[0][1] = std::sin(tmp_radians);
        rotation[1][0] = -rotation[0][1];
        view_rotate_matrix = rotation * view_rotate_matrix;
        ExtractUVWE();
        ComposeViewMatrix();
    }

    bool Viewer::IsPositiveBoth(scalar _z_near, scalar _z_far)
    {
        bool ret = false;
        if (_z_near > 0 && _z_far > 0)
        {
            ret = true;
        }

        return ret;
    }

    bool Viewer::IsValidFovyRatio(scalar _fovy, scalar _ratio)
    {
        bool ret = false;
        if (_ratio > 0 && _fovy > 0 && _fovy < 90)
        {
            ret = true;
        }

        return ret;
    }

    void Viewer::PrintProjectionMatrix()
    {
        std::cout << "Projection: \n";
        PrintMat4(projection_matrix);
    }

    void Viewer::Orthographic(scalar _left, scalar _right, scalar _bottom, scalar _top, scalar _near, scalar _far)
    {
        /*
         this marching implementation has 1e-6 precision in last column elements
         compare with glm::ortho because of matirx multiplation
         the better implementation is to construct one matrix with symbol solution

         new: after assign values with symbol solution. nothing changed.
         still have little difference less than 1e-8, and totally same with marching approach
         strange!!
           here's a test example
            glm::mat4 projection = glm::ortho(-84.431, 100.43414,-4311.15, -0.43, -3.342, 512.41152);
            PrintMat4(projection);
            PrintMat4(myortho(-84.431, 100.43414,-4311.15, -0.43, -3.342, 512.41152));
            PrintMat4(myortho_marching(-84.431, 100.43414,-4311.15, -0.43, -3.342, 512.41152));
         */
        /*
                glm::mat4 myortho_marching(float _left, float _right, float _bottom, float _top, float _near, float _far)
                {
                    float z_near, z_far, x_left, x_right,y_top, y_bottom;
                    z_near = -_near; z_far = -_far;

                    x_left = _left; x_right = _right; y_top = _top; y_bottom = _bottom;

                    glm::mat4 trans(1.0);
                    trans[3] = glm::vec4(-(x_left + x_right) / 2.0, -(y_top + y_bottom) / 2.0, \
                                    -(z_near + z_far) / 2.0, 1.0);

                    glm::mat4 scl(1.0);
                    scl[0][0] = 2.0 / (x_right - x_left);
                    scl[1][1] = 2.0 / (y_top - y_bottom);
                    // result in NDC space is left-hand, beacause z is negative
                    // as shown in http://www.songho.ca/opengl/files/gl_projectionmatrix01.png
                    scl[2][2] = 2.0 / (z_far - z_near);

                    glm::mat4 projection_matrix = scl * trans;
                    return projection_matrix;
                }

                glm::mat4 myortho(float _left, float _right, float _bottom, float _top, float _near, float _far)
                {
                    float z_near, z_far, x_left, x_right,y_top, y_bottom;
                    z_near = -_near; z_far = -_far;

                    x_left = _left; x_right = _right; y_top = _top; y_bottom = _bottom;

                    glm::mat4 trans(1.0);
                    trans[3] = glm::vec4(-(x_left + x_right) / 2.0, -(y_top + y_bottom) / 2.0, \
                                    -(z_near + z_far) / 2.0, 1.0);


                    glm::mat4 projection_matrix(1.0);
                    float tmp_x = x_right - x_left;
                    float tmp_y = y_top - y_bottom;
                    float tmp_z = z_far - z_near;

                    projection_matrix[0][0] = 2.0 / tmp_x;
                    projection_matrix[1][1] = 2.0 / tmp_y;
                    // result in NDC space is left-hand, beacause z is negative
                    // as shown in http://www.songho.ca/opengl/files/gl_projectionmatrix01.png
                    projection_matrix[2][2] = 2.0 / tmp_z;
                    projection_matrix[3] = glm::vec4(-(x_left + x_right) / tmp_x, -(y_top + y_bottom) / tmp_y, \
                                                     -(z_near + z_far) / tmp_z, 1.0);



                    return projection_matrix;
                }

         */

        /*
         * latex online render: https://www.codecogs.com/latex/eqneditor.php
        * translation =
            \begin{pmatrix}
            1 & 0 & 0 & -\frac{l+r}{2} \\
            0 & 1 & 0 & -\frac{b+t}{2}\\
            0 & 0 & 1 & -\frac{n+f}{2}\\
            0 & 0 & 0 & 1
            \end{pmatrix}
         * scale =
            \begin{pmatrix}
            \frac{2}{r-l} & 0 & 0 & 0 \\
            0 & \frac{2}{t-b} & 0 & 0\\
            0 & 0 & \frac{2}{f-n} & 0\\
            0 & 0 & 0 & 1
            \end{pmatrix}
         * ortho_projection_matrix = scale * translation =
            \begin{pmatrix}
            \frac{2}{r-l} & 0 & 0 & \frac{l+r}{l-r} \\
            0 & \frac{2}{t-b} & 0 & \frac{b+t}{b-t}\\
            0 & 0 & \frac{2}{f-n} & \frac{n+f}{n-f}\\
            0 & 0 & 0 & 1
            \end{pmatrix}
        */



        assert(IsPositiveBoth(_near, _far));
        proType = ORTHOGRAPHIC;

        z_near = -_near; z_far = -_far;
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

    void Viewer::frustum(scalar _left, scalar _right, scalar _bottom, scalar _top, scalar _near, scalar _far)
    {
        /* latex online render: https://www.codecogs.com/latex/eqneditor.php
         * first "squeeze" frustum volume into suqare volume
         * then then apply orthographic projection
         * suqeeze =
            \begin{pmatrix}
            n & 0 & 0 & 0 \\
            0 & n & 0 & 0\\
            0 & 0 & n+f & -nf\\
            0 & 0 & 1 & 0
            \end{pmatrix}
         * ortho =
            \begin{pmatrix}
            \frac{2}{r-l} & 0 & 0 & \frac{l+r}{l-r} \\
            0 & \frac{2}{t-b} & 0 & \frac{b+t}{b-t}\\
            0 & 0 & \frac{2}{f-n} & \frac{n+f}{n-f}\\
            0 & 0 & 0 & 1
            \end{pmatrix}
         * ret = ortho * squeeze =
            \begin{pmatrix}
            \frac{2n}{r-l} & 0 & \frac{l+r}{l-r} & 0 \\
            0 & \frac{2n}{t-b} & \frac{b+t}{b-t} & 0\\
            0 & 0 & \frac{n+f}{f-n} & -\frac{2nf}{f-n}\\
            0 & 0 & 1 & 0
            \end{pmatrix}
         * ret = -ret;
         *   In our implementation,
         *   z_znear, z_far are negative while fuction args _near and _far are positive.
         *   Before transformaion, we make z values negative manualy.
         *   our transformation only squeeze and ortho-pro.
         *
         *   In opengl implemetation,
         *   Actually,  z values passing from function are
         *   not forced into negative values firstly. Reversely, convert them to
         *   negative values by projection matrix. That's why our projection transformation
         *   is a little difference with [here](http://www.songho.ca/opengl/files/gl_projectionmatrix_eq16.png)
         *
         *   we need to pass it to opengl with consistence.
         *   Therefore, we take opposite operation lastly
         *
        */
        assert(IsPositiveBoth(_near, _far));
        proType = PERSPECTIVE;

        z_near = -_near; z_far = -_far;
        x_left = _left; x_right = _right; y_top = _top; y_bottom = _bottom;

        Mat4 persp2ortho(1.0);
        persp2ortho[0][0] = persp2ortho[1][1] = z_near;
        persp2ortho[2][2] = z_near + z_far; persp2ortho[3][3] = 0.0;
        persp2ortho[2][3] = 1.0;
        persp2ortho[3][2] = -z_near * z_far;

        Mat4 ortho(1.0);
        scalar tmp_x = x_right - x_left;
        scalar tmp_y = y_top - y_bottom;
        scalar tmp_z = z_far - z_near;
        ortho[0][0] = 2.0 / tmp_x;
        ortho[1][1] = 2.0 / tmp_y;
        // result in NDC space is left-hand, beacause z is negative
        // as shown in http://www.songho.ca/opengl/files/gl_projectionmatrix01.png
        ortho[2][2] = 2.0 / tmp_z;
        ortho[3] = Vec4(-(x_left + x_right) / tmp_x, -(y_top + y_bottom) / tmp_y, \
                                         -(z_near + z_far) / tmp_z, 1.0);

        projection_matrix = ortho * persp2ortho;
        projection_matrix = -projection_matrix;
    }

    void Viewer::Perspective(scalar _fovy, scalar _aspect, scalar _zNear, scalar _zFar)
    {
        /* latex online render: https://www.codecogs.com/latex/eqneditor.php
         * first "squeeze" frustum volume into suqare volume
         * then then apply orthographic projection
         * ret =
            \begin{pmatrix}
            \frac{2n}{r-l} & 0 & \frac{l+r}{l-r} & 0 \\
            0 & \frac{2n}{t-b} & \frac{b+t}{b-t} & 0\\
            0 & 0 & \frac{n+f}{f-n} & -\frac{2nf}{f-n}\\
            0 & 0 & 1 & 0
            \end{pmatrix}
         * l = -r, b = -t;
         * aspect ration = \frac{r}{t}
         * tan(\frac{aspect}{2}) = \frac{t}{abs(n)}
         * ret =
            \begin{pmatrix}
            \frac{n}{r} & 0 & 0 & 0 \\
            0 & \frac{n}{t} & 0 & 0\\
            0 & 0 & \frac{n+f}{f-n} & -\frac{2nf}{f-n}\\
            0 & 0 & 1 & 0
            \end{pmatrix}
         * ret = -ret;
         *   In our implementation,
         *   z_znear, z_far are negative while fuction args _near and _far are positive.
         *   Before transformaion, we make z values negative manualy.
         *   our transformation only squeeze and ortho-pro.
         *
         *   In opengl implemetation,
         *   Actually,  z values passing from function are
         *   not forced into negative values firstly. Reversely, convert them to
         *   negative values by projection matrix. That's why our projection transformation
         *   is a little difference with [here](http://www.songho.ca/opengl/files/gl_projectionmatrix_eq16.png)
         *
         *   we need to pass it to opengl with consistence.
         *   Therefore, we take opposite operation lastly
        */
        assert(IsPositiveBoth(_zNear, _zFar));
        assert(IsValidFovyRatio(_fovy, _aspect));
        proType = PERSPECTIVE;

        scalar tan_fovyh = std::tan(_fovy / 2.0);
        aspect = _aspect;
        z_near = -_zNear; z_far = -_zFar;
        y_top =  std::abs(z_near) * tan_fovyh;  y_bottom = -y_top;
        x_right = _aspect * tan_fovyh * std::abs(z_near);   x_left = -x_right;

        // or directly call function
        // frustum(x_left, x_right, y_bottom, y_top, -z_near, -z_far);
        projection_matrix = Mat4(1.0);
        scalar tmp = 1 / (z_far - z_near);
        projection_matrix[0][0] = z_near / x_right;
        projection_matrix[1][1] = z_near / y_top;
        projection_matrix[2][2] = (z_near + z_far) * tmp;
        projection_matrix[3][2] = -2.0 * z_near * z_far * tmp;
        projection_matrix[3][3] = 0.0;
        projection_matrix[2][3] = 1.0;

        projection_matrix = -projection_matrix;
    }
















}
