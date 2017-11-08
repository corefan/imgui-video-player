#include <Eigen/Core>
#include <iostream>
#include "ViewMatrix.h"

main()
{
	std::cout << "Testing Renderers" << std::endl;	

	TransformationMatrix tm;
	vec3f eye(0.0f,1.0f,0.0f);
	vec3f target;
	vec3f up;

	eye.Random(); target.Random(); up.Random();

	tm.LookAt_rh(eye, target, up);
	std::cout << tm << std::endl;

	tm.Random();

	tm.LookAt_rh_fast(eye, target, up);

	std::cout << std::endl << tm << std::endl;
	return 1;
}