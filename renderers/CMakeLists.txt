cmake_minimum_required(VERSION 3.0)

set(renderers_sources Framebuffer.cpp ViewMatrix.cpp)

add_library(renderers ${renderers_sources})

target_link_libraries( renderers
  Eigen3::Eigen
)

add_executable(TestRenderers
	test.cpp
)

target_link_libraries( TestRenderers
  Eigen3::Eigen
  renderers
)