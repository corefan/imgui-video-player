#include "ViewMatrix.h"
#include <iostream>

void TransformationMatrix::LookAt_rh(vec3f e, vec3f t, vec3f u)
{
	vec3f zaxis = (e - t).normalized();
	vec3f xaxis = u.cross(zaxis).normalized();
	vec3f yaxis = zaxis.cross(xaxis);

	// TODO: figure out how to make this more efficient with mapping
	Eigen::Matrix4f::block<4,1>(0,0) << xaxis, -xaxis.dot(e); 
	Eigen::Matrix4f::block<4,1>(0,1) << yaxis, -yaxis.dot(e);
	Eigen::Matrix4f::block<4,1>(0,2) << zaxis, -zaxis.dot(e);

	// TODO figure out when this operation is unnecessary
	Eigen::Matrix4f::block<4,1>(0,3) << 0,0,0,1;
}

void TransformationMatrix::LookAt_rh_fast(vec3f e, vec3f t, vec3f u)
{
	/* This uses Eigen's Map to do math in place, thereby avoiding time consuming copies !
		TODO: test if it actually works? Also it's damn ugly. maybe clean it up with a macro?
		here's an idea! use "using"... should work because inheritance!
	*/
	Eigen::Matrix4f::block<3,1>(0,2) << (e - t).normalized();
	Eigen::Matrix4f::block<1,1>(3,2) << -Map3f(Eigen::Matrix4f::col(2).data(),3).dot(e); // zaxis
	
	Eigen::Matrix4f::block<3,1>(0,0) << u.cross( Map3f(Eigen::Matrix4f::col(2).data(),3).normalized()  );
	Eigen::Matrix4f::block<1,1>(3,0) << -Map3f(Eigen::Matrix4f::col(0).data(),3).dot(e); // xaxis

	Eigen::Matrix4f::block<3,1>(0,1) <<
		Map3f(Eigen::Matrix4f::col(0).data(),3).cross( Map3f(Eigen::Matrix4f::col(2).data(),3) );
	Eigen::Matrix4f::block<1,1>(3,1) << -Map3f(Eigen::Matrix4f::col(1).data(),3).dot(e); // yaxis
}