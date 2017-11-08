#include <Eigen/Core>
#include <iostream>
#include "ViewMatrix.h"

main()
{
	std::cout << "Testing Renderers" << std::endl;	

	TransformationMatrix tm;
	vec3f eye;
	vec3f target;
	vec3f up;

	eye= vec3f::Random(); target = vec3f::Random(); up = vec3f::Random();
	std::cout << "eye:\n" << eye << std::endl;
	std::cout << "up:\n" << up << std::endl;
	std::cout << "target:\n" << target << std::endl;

	tm.LookAt_rh(eye, target, up);

	std::cout << tm._transform << std::endl;

	tm.LookAt_rh_fast(eye, target, up);


	std::cout << std::endl << tm._transform << std::endl;
	return 1;
}