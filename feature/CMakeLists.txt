cmake_minimum_required(VERSION 3.0)

set(feature_sources mser.cpp )

add_library(feature ${feature_sources})
target_link_libraries(feature
  vlfeat)
