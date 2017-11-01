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

pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(AVFORMAT REQUIRED libavformat)
pkg_search_module(SWSCALE REQUIRED libswscale)
include_directories(
  .
   ${GLUT_INCLUDE_DIRS}
   ${GLEW_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/vendor"
  "${PROJECT_SOURCE_DIR}/video-player"
  "${PROJECT_SOURCE_DIR}/libs/glfw/include/"
  ${OPENGL_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIRS})

set( source_files
  src/main.cpp
  src/imgui_demo.cpp
  src/imgui_draw.cpp
  src/imgui_impl_glfw_gl3.cpp
  src/imgui.cpp )

LINK_DIRECTORIES(${GLFW_LIBRARY_DIR})

# add_subdirectory( "${PROJECT_SOURCE_DIR}/video-player" )

add_executable( app ${source_files})

target_link_libraries( app
  # videoPlayer
  pthread
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
