#ifndef GEOMETRY_H
#define GEOMETRY_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace viewer {

// if you want a double-precision
// set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -DUSING_FLOAT64")
    namespace geometry {
        #ifdef USING_FLOAT64
            typedef double scalar;
        #else
            typedef float scalar;
        #endif
        typedef std::numeric_limits<scalar> dfl;

        using namespace glm;
        // vector types: https://glm.g-truc.net/0.9.9/api/a00281.html
        // matrix types:  https://glm.g-truc.net/0.9.9/api/a00283.html
        // column major, mat[j][i], i-th row, j-th column
        typedef glm::vec< 2, scalar, glm::defaultp > 	Vec2;
        typedef glm::vec< 3, scalar, glm::defaultp > 	Vec3;
        typedef glm::vec< 4, scalar, glm::defaultp > 	Vec4;


        typedef glm::mat< 2, 2, scalar, glm::defaultp > 	Mat2;
        typedef glm::mat< 2, 2, scalar, glm::defaultp > 	Mat2x2;
        typedef glm::mat< 2, 3, scalar, glm::defaultp > 	Mat2x3; // 2 columns, 3 rows
        typedef glm::mat< 2, 4, scalar, glm::defaultp > 	Mat2x4;

        typedef glm::mat< 3, 3, scalar, glm::defaultp > 	Mat3;
        typedef glm::mat< 3, 3, scalar, glm::defaultp > 	Mat3x3;
        typedef glm::mat< 3, 4, scalar, glm::defaultp > 	Mat3x4;

        typedef glm::mat< 4, 2, scalar, glm::defaultp > 	Mat4x2;
        typedef glm::mat< 4, 3, scalar, glm::defaultp > 	Mat4x3;
        typedef glm::mat< 4, 4, scalar, glm::defaultp > 	Mat4x4;
        typedef glm::mat< 4, 4, scalar, glm::defaultp > 	Mat4;


    }


}

#endif // GEOMETRY_H
