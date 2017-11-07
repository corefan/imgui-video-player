cmake_minimum_required(VERSION 3.0)

set(video_player_sources
video_player.cpp)

add_library(videoPlayer ${video_player_sources})
target_link_libraries(videoPlayer
	framebuffer
	Eigen3::Eigen
	feature)
