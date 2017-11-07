#ifndef __mser_cpu_extractor_h__
#define __mser_cpu_extractor_h__

#include <Eigen/Core>
#include "vendor/mser/mser.h"


/*
  // from the mser documentation:
  /// @param[in] delta DELTA parameter of the MSER algorithm. Roughly speaking, the stability of a
  int delta;
  /// region is the relative variation of the region area when the intensity is changed by delta.
  /// @param[in] minArea Minimum area of any stable region relative to the image domain area.
  double minArea;
  /// @param[in] maxArea Maximum area of any stable region relative to the image domain area.
  double maxArea;
  /// @param[in] maxVariation Maximum variation (absolute stability score) of the regions.
  double maxVariation;
  /// @param[in] minDiversity Minimum diversity of the regions. When the relative area of two
  /// nested regions is below this threshold, then only the most stable one is selected.
  double minDiversity;

  bool use8ConnectedPixels;
  /// @param[in] eight Use 8-connected pixels instead of 4-connected.
*/

 

class mser_cpu_extractor : public  MSER
{
public:
 
size_t Run(uint8_t* image_data, size_t w, size_t h);
void RegionsFirstMomentsToXYZ(Eigen::VectorXf& v);

private:
  size_t last_w, last_h;
  std::vector<MSER::Region> regions;
};

#endif
