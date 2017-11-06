cmake_minimum_required(VERSION 3.0)

find_package(PkgConfig REQUIRED)

set( CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} -mtune=native -O3 -std=c11 -pthread" )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

message(STATUS  "module path" ${CMAKE_MODULE_PATH} )
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(FFmpeg REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(AVFORMAT REQUIRED libavformat)
pkg_search_module(SWSCALE REQUIRED libswscale)
include_directories(
  .
   ${GLUT_INCLUDE_DIRS}
   ${GLEW_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/ui"
  "${PROJECT_SOURCE_DIR}/libs/ImGuiDock"
  "${PROJECT_SOURCE_DIR}/vendor"
  "${PROJECT_SOURCE_DIR}/video-player"
  "${PROJECT_SOURCE_DIR}/libs/glfw/include/"
  "${PROJECT_SOURCE_DIR}/vendor/glm-0.9.7.1/"
  ${OPENGL_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIRS})

set( source_files
  ui/main.cpp
  ui/imgui_demo.cpp
  ui/imgui_draw.cpp
  ui/imgui_impl_glfw_gl3.cpp
  ui/imgui_impl_glfw_gl3_01.cpp
  ui/imgui.cpp
  ui/testgl.cpp
  ui/gl_state.cpp
  ui/Framebuffer.cpp)

configure_file(ui/suzanne.obj suzanne.obj)
configure_file(ui/uvmap.DDS uvmap.DDS)

LINK_DIRECTORIES(${GLFW_LIBRARY_DIR})

add_subdirectory( "${PROJECT_SOURCE_DIR}/vendor/VLFeat" )
add_subdirectory( "${PROJECT_SOURCE_DIR}/feature" )
add_subdirectory( "${PROJECT_SOURCE_DIR}/vendor/mser" )
# add_subdirectory( "${PROJECT_SOURCE_DIR}/vendor/glm-0.9.7.1" )

add_executable( app ${source_files})

target_link_libraries( app
  vlfeat
  feature
  pthread
  mser
  ${FFMPEG_LIBAVCODEC}
  ${FFMPEG_LIBAVFORMAT}
  ${AVFORMAT_LIBRARIES}
  ${SWSCALE_LIBRARIES}
  ${FFMPEG_LIBAVUTIL}
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${GLFW_LIBRARIES}
  ${GLUT_LIBRARY}
  ${OPENGL_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  ${FFMPEG_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${CMAKE_DL_LIBS}
)
