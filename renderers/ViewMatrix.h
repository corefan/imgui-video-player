#ifndef __VIEWMATRIX_H
#define __VIEWMATRIX_H

#include <Eigen/Core>
#include <Eigen/Geometry>


using vec3f = Eigen::Matrix<float,3,1>;
using vec4f = Eigen::Matrix<float,4,1>;
using mat4f = Eigen::Matrix4f;
using Map3f = Eigen::Map<const vec3f>;

class TransformationMatrix 
{
public:
	TransformationMatrix()  : _target{0,0,0}, _eye{0,0,3}, _up{0,0,1} 
	{
		_transform.block<4,1>(0,3) << 0,0,0,1;
	}
	void LookAt_rh(const vec3f& eye, const vec3f& target, const vec3f& up);
	void LookAt_rh_fast(const vec3f& e, const vec3f& t, const  vec3f& u);
	// void LookAt_rh_fast();


	// void SetTarget(const vec3f target) { _target = target; }
	// void setEye(const vec3f eye) { _eye = eye; }
	// void setUp(const vec3f up) { _up = up; }

	// const vec3f Target() { return _target; }
	// const vec3f Eye() { return _eye; }
	// const vec3f Up() { return _up; }
	Eigen::Matrix4f _transform;

private:
	vec3f _target;
	vec3f _eye;
	vec3f _up;
};	

#endif