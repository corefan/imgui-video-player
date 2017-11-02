#include "mser.h"

mser_cpu_context::mser_cpu_context(size_t w, size_t h, size_t c)
: _dims{0,0,0,0,0} , _ndims{0} , MserFilt{nullptr}
{
  _ndims = 0;
  // TODO:
  const AVPixelFormat dest_format = AV_PIX_FMT_GRAY8;
  if (dest_format == AV_PIX_FMT_GRAY8)
  {
    _ndims = 3; _dims[0] = h; _dims[1] = w; _dims[2] = 1;
  }

  MserFilt = vl_mser_new(_ndims, _dims);
}

mser_cpu_context::~mser_cpu_context()
{
  if (MserFilt != nullptr) vl_mser_delete(MserFilt);
}


bool mser_cpu_context::Check() const
{
  return  _ndims > 0 && MserFilt != nullptr ? true : false;
}
