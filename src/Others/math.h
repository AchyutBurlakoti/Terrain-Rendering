#ifndef MATH_HPP
#define MATH_HPP
#include <iostream>
#include <math.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


glm::mat4 perspective(float fovy, float aspect_ratio, float near_plane, float far_plane)
{
    glm::mat4 id(1.0f);
    float tan_fovy = std::sin(fovy)/std::cos(fovy);
    float top = near_plane * tan_fovy;
    float right = aspect_ratio * top;
        
    id[0][0] = near_plane / right;
    id[1][1] = near_plane / top;
    id[2][3] = -1;
    id[3][2] = - 2 * far_plane * near_plane / (far_plane - near_plane);
    id[2][2] = -(far_plane+near_plane) / (far_plane - near_plane);
    id[3][3] = 0;
    return id;
}

glm::vec3 cross(glm::vec3 const& v1, glm::vec3 const& v2)
{
    return glm::vec3(v1.y * v2.z - v1.z * v2.y,
                    v1.z * v2.x - v1.x * v2.z,
                    v1.x * v2.y - v1.y * v2.x);
}

glm::vec3 normalize(glm::vec3 v1)
{
    float magnitude = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
    return glm::vec3(v1.x / magnitude, v1.y / magnitude, v1.z / magnitude);
}

float dot(glm::vec3 v1, glm::vec3 v2)
{
    return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
}

glm::mat4 lookAt(glm::vec3 const& eye, glm::vec3 const& center, glm::vec3 const& up)
{
	glm::vec3 const f(normalize(eye - center));
	glm::vec3 const s(normalize(cross(up, f)));
	glm::vec3 const u(cross(f, s));

	glm::mat4 Result(1.0f);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = f.x;
	Result[1][2] = f.y;
	Result[2][2] = f.z;
	Result[3][0] = -dot(s, eye);
	Result[3][1] = -dot(u, eye);
	Result[3][2] = -dot(f, eye);
	return Result;
}

glm::mat4 translate(glm::mat4 const& m, glm::vec3 const& v)
{
	glm::mat4 Result(m);
	Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return Result;
}

glm::mat4 rotate(glm::mat4 const& m, float angle, glm::vec3 const& v)
{
	float const a = angle;
	float const c = cos(a);
	float const s = sin(a);

	glm::vec3 axis(normalize(v));
	glm::vec3 temp((float(1) - c) * axis);

	glm::mat4 Rotate;
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];

	glm::mat4 Result;
	Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
	Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
	Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
	Result[3] = m[3];
	return Result;
}

    #endif