
#pragma once

#include <glm/detail/type_vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/projection.hpp>

namespace math
{
    template <typename T, glm::precision P>
    static inline glm::detail::tvec3<T, P> forward(glm::detail::tquat<T, P> const & q)
    {
        // equivalent to
        // return q * vec3(0.0, 0.0, 1.0) * conjugate(q);
        //
        auto x = 2 * (q.x * q.z + q.w * q.y);
        auto y = 2 * (q.y * q.z - q.w * q.x);
        auto z = 1 - 2 * (q.x * q.x + q.y * q.y);
        return glm::normalize(glm::detail::tvec3<T, P>(x, y, z));
    }

    template <typename T, glm::precision P>
    static inline glm::detail::tvec3<T, P> up(glm::detail::tquat<T, P> const & q)
    {
        // equivalent to
        // return q * vec3(0.0, 1.0, 0.0) * conjugate(q);
        //
        auto x = 2 * (q.x * q.y - q.w * q.z);
        auto y = 1 - 2 * (q.x * q.x + q.z * q.z);
        auto z = 2 * (q.y * q.z + q.w * q.x);
        return glm::normalize(glm::detail::tvec3<T, P>(x, y, z));
    }

    template <typename T, glm::precision P>
    static inline glm::detail::tvec3<T, P> right(glm::detail::tquat<T, P> const & q)
    {
        // equivalent to
        // return q * vec3(1.0, 0.0, 0.0) * conjugate(q);
        //
        auto x = 1 - 2 * (q.y * q.y + q.z * q.z);
        auto y = 2 * (q.x * q.y + q.w * q.z);
        auto z = 2 * (q.x * q.z - q.w * q.y);
        return glm::normalize(glm::detail::tvec3<T, P>(x, y, z));
    }

    template <typename T, glm::precision P>
    static inline glm::detail::tquat<T, P> rotate_local(glm::detail::tquat<T, P> const & q,
                                                        T angle_degrees,
                                                        glm::detail::tvec3<T, P> const & v)
    {
        auto angle_axis = glm::normalize(glm::angleAxis(angle_degrees, v));
        return glm::normalize(q * angle_axis);
    }

    template <typename T, glm::precision P>
    static inline glm::detail::tquat<T, P> rotate_world(glm::detail::tquat<T, P> const & q,
                                                        T angle_degrees,
                                                        glm::detail::tvec3<T, P> const & v)
    {
        auto angle_axis = glm::normalize(glm::angleAxis(angle_degrees, v));
        return glm::normalize(angle_axis * q);
    }

    // From Geometric Tools LibMathematics
    //   DecomposeTwistTimesSwing:
    // Decompose a quaternion into q = q_twist * q_swing, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    template <typename T, glm::precision P>
    static inline void decompose_twist_swing(glm::detail::tquat<T, P> const & q,
                                             glm::detail::tvec3<T, P> const & twist_axis,
                                             glm::detail::tquat<T, P> & swing,
                                             glm::detail::tquat<T, P> & twist)
    {
        auto rotated = glm::rotate(q, twist_axis);
        swing = glm::normalize(glm::rotation(twist_axis, rotated));
        twist = glm::normalize(q * glm::conjugate(swing));
    }

    // From Geometric Tools LibMathematics
    //   DecomposeSwingTimesTwist:
    // Decompose a quaternion into q = q_swing * q_twist, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    template <typename T, glm::precision P>
    static inline void decompose_swing_twist(glm::detail::tquat<T, P> const & q,
                                             glm::detail::tvec3<T, P> const & twist_axis,
                                             glm::detail::tquat<T, P> & swing,
                                             glm::detail::tquat<T, P> & twist)
    {
        auto rotated = glm::rotate(q, twist_axis);
        swing = glm::normalize(glm::rotation(twist_axis, rotated));
        twist = glm::normalize(glm::conjugate(swing) * q);
    }

    // Based on code from David Eberly's "Constrained Quaternions Using Euler Angles"
    template <typename T, glm::precision P>
    static inline glm::detail::tquat<T, P> closest_x(glm::detail::tquat<T, P> const & q,
                                                     T min_angle, T max_angle)
    {
        min_angle = min_angle * T(0.5);
        max_angle = max_angle * T(0.5);
        glm::detail::tquat<T, P> ret = q;

        auto length_sqr = q.w*q.w + q.x*q.x;

        // if no length, any solution will work
        if (length_sqr <= glm::epsilon<T>())
        {
            return ret;
        }

        auto length_inv = glm::inversesqrt(length_sqr);

        auto cos_min = glm::cos(min_angle);
        auto cos_max = glm::cos(max_angle);
        auto cos_diff = cos_max - cos_min;

        auto sin_min = glm::sin(min_angle);
        auto sin_max = glm::sin(max_angle);
        auto sin_diff = sin_max - sin_min;

        if (((q.w - cos_min) * sin_diff >= (q.x - sin_min) * cos_diff) ||
            ((q.w + cos_min) * sin_diff <= (q.x - sin_min) * cos_diff))
        {
            ret.w = q.w * length_inv;
            ret.x = q.x * length_inv;
            return glm::normalize(ret);
        }

        // find the closest angle within the bounds
        // compare q and -q to the min angle
        auto dot_min = q.w*cos_min + q.x*sin_min;
        if (dot_min < T(0.0))
        {
            cos_min = -cos_min;
            sin_min = -sin_min;
            dot_min = -dot_min;
        }

        // compare q and -q to the max angle
        auto dot_max = q.w*cos_max + q.x*sin_max;
        if (dot_max < T(0.0))
        {
            cos_max = -cos_max;
            sin_max = -sin_max;
            dot_max = -dot_max;
        }

        ret.w = (dot_max >= dot_min) ? cos_max : cos_min;
        ret.x = (dot_max >= dot_min) ? sin_max : sin_min;

        return glm::normalize(ret);
    }
    
    template <typename T, glm::precision P>
    static inline glm::detail::tquat<T, P> limit_rotation_xaxis(glm::detail::tquat<T, P> const & q,
                                                                T max_degrees)
    {
        const auto x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::detail::tquat<T, P> swing;
        glm::detail::tquat<T, P> twist;
        math::decompose_swing_twist(q, x_axis, swing, twist);
        twist = math::closest_x(twist, -max_degrees, max_degrees);
        return swing * twist;
    }
}
