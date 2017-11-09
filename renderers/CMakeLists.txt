cmake_minimum_required(VERSION 3.0)

set(renderers_sources 
	Framebuffer.cpp
	ViewMatrix.cpp
	GLUtils.cpp
	Points3d.cpp
	)

add_library(renderers ${renderers_sources})

target_link_libraries( renderers
  Eigen3::Eigen
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${GLFW_LIBRARIES}
  ${GLUT_LIBRARY}

)

add_executable(TestRenderers
	test.cpp
)

target_link_libraries( TestRenderers
  Eigen3::Eigen
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${GLFW_LIBRARIES}
  ${GLUT_LIBRARY}

  renderers
)