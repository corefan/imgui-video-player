#ifndef _VIDEO_PLAYER_H_
#define _VIDEO_PLAYER_H_

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

// GL frame renderer
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "readerwriterqueue/atomicops.h"
#include "readerwriterqueue/readerwriterqueue.h"

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
  void RenderFrame_0(AVFrame* frame);
  void RenderFrame_1(AVFrame* frame);

  GL3FrameRenderer_Texture2D(const size_t w, const size_t h)
  : width{w}, height{h}
  {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  }

private:
  size_t width;
  size_t height;
protected:
  GLuint tex;

};

// TODO: don't use inheritance like this
template<class Renderer>
class GL3VideoPlayer : Renderer
{
public:
  GL3VideoPlayer(size_t w, size_t h,  GLFWwindow* window)
  :  Renderer(w,h) , format_ctx{nullptr} ,
     codec_ctx{nullptr} ,
     _stream_open{false} ,
     video_stream_index{-1} ,
     picture{nullptr} , picture_buffer{nullptr} ,
     stream{nullptr} , output_ctx{nullptr} ,
     codec{nullptr}
  {
    status = "Initialzing...";

    if (! _init )
    {
      av_register_all();
      avformat_network_init();
      // av_log_set_level( AV_LOG_DEBUG );

      format_ctx = avformat_alloc_context();
      output_ctx = avformat_alloc_context();
      if (format_ctx == nullptr || output_ctx == nullptr)
      {
        std::cout << "FFMpegStreamer: avformat_alloc_context failed" << std::endl;
        return;
      }

      codec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
      if (codec == nullptr)
      {
          std::cout << "FFMpegStreamer: could not find codec" << std::endl;
          return;
      }

      codec_ctx = avcodec_alloc_context3(codec);
      if (codec_ctx == nullptr)
      {
          std::cout << "FFMpegStreamer: could not allocate codec_ctx" << std::endl;
          return;
      }

      if (avcodec_open2(codec_ctx, codec, nullptr) < 0)
      {
        std::cout << "avcodec_open2 failed" << std::endl;
        return;
      }

      av_init_packet(&packet);

      format_ctx = avformat_alloc_context();
      if (format_ctx == nullptr )
      {
        std::cout << "avformat_alloc_context failed" << std::endl;
        return;
      }

      picture = av_frame_alloc();
      _rgb_frame = av_frame_alloc();
      if (picture == nullptr || _rgb_frame == nullptr)
      {
        std::cout << "av_frame_alloc failed" << std::endl;
        return;
      }

      _init = true;
      status = "Initialized.";
    }
  }

  ~GL3VideoPlayer()
  {
    if (StreamIsOpen())
    {
      av_read_pause(format_ctx);
      avio_close(output_ctx->pb);
    }

    if ( _init )
    {
      avformat_network_deinit();
    }

    if (codec_ctx != nullptr)
    {
      avcodec_close( codec_ctx );
      avcodec_free_context( &codec_ctx);
      av_freep( &codec_ctx);
    }

    if (picture != nullptr) av_frame_unref(picture);
    av_freep(&picture);
    if (_rgb_frame != nullptr) av_frame_unref(_rgb_frame);
    av_freep(&_rgb_frame);
    if (picture_buffer != nullptr) av_free(picture_buffer);
    if (output_ctx != nullptr) avformat_free_context(output_ctx);
    if (format_ctx != nullptr) avformat_free_context(format_ctx);
  }

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
};

#include "video_player.cpp"

#endif // _VIDEO_PLAYER_H_
