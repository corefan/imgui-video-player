#ifndef __VIEWMATRIX_H
#define __VIEWMATRIX_H

#include <Eigen/Core>
#include <Eigen/Geometry>


using vec3f = Eigen::Vector3f;
using vec4f = Eigen::Vector4f;
using mat4f = Eigen::Matrix4f;
using Map3f = Eigen::Map<const vec3f>;

class TransformationMatrix : public Eigen::Matrix4f
{
public:
	TransformationMatrix()
	{
		Eigen::Matrix4f::block<4,1>(0,3) << 0,0,0,1;
	}
	void LookAt_rh(vec3f target);
	void LookAt_rh(vec3f eye, vec3f target, vec3f up);
	void LookAt_rh_fast(vec3f e, vec3f t, vec3f u);
};	

#endif