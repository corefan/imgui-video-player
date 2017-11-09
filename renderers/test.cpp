#include <Eigen/Core>
#include <iostream>
#include "ViewMatrix.h"
#include "Points3d.h"
static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}


main()
{
	    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* gl_window_ctx = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(gl_window_ctx);
    glfwSwapInterval(1); // Enable vsync
    if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      return 1;
    }
    
	std::cout << "Testing Renderers" << std::endl;	


	TransformationMatrix tm;

	Points3d pp;

	vec3f eye;
	vec3f target;
	vec3f up; up << 1,2,3;

	std::cout << "up: " << up << std::endl;
	std::cout << "up[0]: " << up.data()[2] << std::endl;

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