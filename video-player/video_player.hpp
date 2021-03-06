#ifndef _VIDEO_PLAYER_H_
#define _VIDEO_PLAYER_H_

#include <algorithm>
#include <cmath>
#include <future>
#include <thread>
#include <iostream>
#include <set>
#include <SDL2/SDL.h>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
}

#include "feature/mser_cpu_extractor.h"

// GL frame renderer
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "readerwriterqueue/atomicops.h"
#include "readerwriterqueue/readerwriterqueue.h"

#include <Eigen/Core>

#include "renderers/Framebuffer.h"

using namespace moodycamel;
using Clock = std::chrono::steady_clock;
using Interval = std::chrono::nanoseconds;

struct TimeSegment
{
  std::chrono::time_point<Clock> t0;
  std::chrono::time_point<Clock> t1;

  void Start() { t0 = Clock::now(); }
  void Stop() { t1 = Clock::now(); }
  void Point() { t0 = t1; t1 = Clock::now(); }
  size_t Duration() { return std::chrono::duration_cast<Interval>(t1 - t0).count(); }
  TimeSegment() { t0 = Clock::now(); t1 = t0; }
};

class GL3FrameRenderer_Texture2D
{
public:
  void RenderFrame_0_red(AVFrame* frame);
  void RenderFrame_1_red(AVFrame* frame);
  void RenderFrame_2_red(uint8_t* frame);

  GL3FrameRenderer_Texture2D(const size_t w, const size_t h)
  : width{w}, height{h}
  {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width,height,0,GL_RED,GL_UNSIGNED_BYTE, nullptr);

    glGenTextures(1, &mers_tex);
    glBindTexture(GL_TEXTURE_2D, mers_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width,height,0,GL_RED,GL_UNSIGNED_BYTE, nullptr);
  }

private:
  size_t width;
  size_t height;
protected:
  GLuint tex;
  GLuint mers_tex;
};

// TODO: don't use inheritance like this
// unless you want to define some function that takes a Renderer as argument
template<class Renderer>
class GL3VideoPlayer : Renderer
{
public:
  GL3VideoPlayer(size_t w, size_t h,  GLFWwindow* window);
  ~GL3VideoPlayer();

  std::future<int> t_OpenStream(std::string url)
  {
    std::future<int> result;
    result = std::async(std::launch::async, &GL3VideoPlayer::i_OpenStream, this, url);
    return result;
  }

  int i_OpenStream(std::string url);

  std::future<int> t_ReadFrame(TimeSegment* tf = nullptr)
  {
    std::future<int> result;
    result = std::async(std::launch::async, &GL3VideoPlayer::i_ReadFrame, this, std::forward<TimeSegment*>(tf));

    return result;
  }

  std::future<int> t_RenderFrame(TimeSegment* tf = nullptr)
  {
    std::future<int> result;
    result = std::async(std::launch::async, &GL3VideoPlayer::i_RenderFrame, this, std::forward<TimeSegment*>(tf));

    return result;
  }

  int i_ReadFrame(TimeSegment* tf = nullptr);
  int i_RenderFrame(TimeSegment* tf = nullptr);

  bool IsInited() { return _init; }
  bool StreamIsOpen() { return _stream_open; }
  GLuint GetTextureId() { return Renderer::tex; }
  GLuint GetTextureId2() { return test_framebuffer.getTexture(); }
  GLuint GetTextureId_mers() { return Renderer::mers_tex; }
  const char* GetStatusString() { return status.c_str(); }
  AVFormatContext* format_ctx;

private:
  AVCodecContext* codec_ctx;
  static bool _init;
  bool _stream_open;
  int video_stream_index;
  AVPacket packet;
  AVFrame* picture;
  uint8_t* picture_buffer;
  AVStream* stream;
  AVFormatContext* output_ctx;
  AVCodec* codec;
  SwsContext *img_convert_ctx;
  AVFrame* _rgb_frame;
  ReaderWriterQueue<AVFrame> _rgb_frame_q{3};
  std::string status;
  static std::set<std::string> open_urls;
  size_t output_width;
  size_t output_height;
  size_t output_channels;
  Framebuffer test_framebuffer;

};

#endif // _VIDEO_PLAYER_H_

#include "video_player.cpp"