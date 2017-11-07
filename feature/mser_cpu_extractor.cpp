#include "mser_cpu_extractor.h"
#include <iostream>


size_t mser_cpu_extractor::Run(uint8_t* image_data, size_t w, size_t h)
{
  MSER::operator ()(image_data, w, h, regions);
  last_w = w; last_h = h;
  return  regions.size();
}

void mser_cpu_extractor::RegionsFirstMomentsToXYZ(Eigen::VectorXf& v)
{
  v.resize(regions.size()*3);
  for (size_t i = 0 ; i < regions.size(); i++)
  {
    float area = static_cast<float>(regions[i].area_);
    float x =  static_cast<float>(regions[i].moments_[0])/area;
    x = -1.0f + 2.0f*x/static_cast<float>(last_w);
    float y =  static_cast<float>(regions[i].moments_[1])/area;
    y = -1.0f + 2.0f*y/static_cast<float>(last_h);

    float z = -0.5f;
    v(3*i) = x; v(3*i + 1) = y; v(3*i + 2) = z;
  }
}