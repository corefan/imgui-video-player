#ifndef __mser_h__
#define __mser_h__

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include "VLFeat/mser.h"
}

struct mser_cpu_context
{
  // Number of threads for feature extraction.
  int num_threads = -1;

  mser_cpu_context(size_t width, size_t heigth, size_t channels);
  ~mser_cpu_context();

  bool Check() const;

  VlMserFilt *MserFilt;

private:
  int _ndims;
  int _dims[5];
};

class mser_cpu_extractor
{
public:
  mser_cpu_extractor(SwsContext *ctx);
  ~mser_cpu_extractor();

private:
  mser_cpu_context vlfeat_ctx;
};

#endif
