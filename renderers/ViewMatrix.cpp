#include "ViewMatrix.h"
#include <iostream>

using Map3f = Eigen::Map<const vec3f>;

void TransformationMatrix::LookAt_rh(const vec3f& e, const  vec3f& t, const vec3f& u)
{
	vec3f zaxis = (e - t).normalized();
	vec3f xaxis = u.cross(zaxis).normalized();
	vec3f yaxis = zaxis.cross(xaxis);

	// TODO: figure out how to make this more efficient with mapping
	_transform.col(0) << xaxis , -xaxis.dot(e); 
	_transform.col(1) << yaxis, -yaxis.dot(e);
	_transform.col(2) << zaxis, -zaxis.dot(e);

}

void TransformationMatrix::LookAt_rh_fast(const vec3f& e, const vec3f& t, const vec3f& u)
{
	/* This uses Eigen's Map to do math in place, thereby avoiding time consuming copies !
		TODO: test if it actually is better
	 ? Also it's damn ugly. 
	*/
	_transform.block<3,1>(0,2) << (e - t).normalized();
	_transform.block<1,1>(3,2) << -Map3f(_transform.col(2).data(),3).dot(e); // zaxis
	
	_transform.block<3,1>(0,0) << u.cross( Map3f(_transform.col(2).data(),3)).normalized()  ;
	_transform.block<1,1>(3,0) << -Map3f(_transform.col(0).data(),3).dot(e); // xaxis
	
	_transform.block<3,1>(0,1) <<
		Map3f(_transform.col(2).data(),3).cross( Map3f(_transform.col(0).data(),3) );
	_transform.block<1,1>(3,1) << -Map3f(_transform.col(1).data(),3).dot(e); // yaxis
}
