/*!
 * @brief Fundamental math utilities.
 */

export module Core.Math;
#define USING_GLM

#ifdef USING_GLM
import glm;

export using Vec2 = glm::vec2;
export using Vec3 = glm::vec3;
export using Vec4 = glm::vec4;
export using Mat2 = glm::mat2;
export using Mat3 = glm::mat3;
export using Mat4 = glm::mat4;

export using glm::operator-;
export using glm::operator+;
export using glm::operator*;

export using Point3 = Vec3;

export namespace Math
{
    using glm::sqrt;
    using glm::ceil;
    using glm::floor;
    using glm::perspective;
    using glm::normalize;
    using glm::lookAt;
    using glm::rotate;
    using glm::radians;
    using glm::translate;
    using glm::scale;
    using glm::cos;
    using glm::sin;
    using glm::tan;
    using glm::dot;
    using glm::cross;
    using glm::clamp;
    using glm::min;
    using glm::max;
}
#elifdef USING_EIGEN
import Eigen;

export using Vec2   = Eigen::Vector2f;
export using Vec3   = Eigen::Vector3f;
export using Vec4   = Eigen::Vector4f;
export using Mat2   = Eigen::Matrix2f;
export using Mat3   = Eigen::Matrix3f;
export using Mat4   = Eigen::Matrix4f;
export using Point3 = Vec3;
#endif
