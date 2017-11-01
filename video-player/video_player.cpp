// #include "video_player.h"

// static members
template<class T>
std::set<std::string> GL3VideoPlayer<T>::open_urls = {};

template<class T>
bool GL3VideoPlayer<T>::_init = false;
/* TODO
int GL3VideoPlayer::CloseStream()
{

}
*/

/* Initialize a video stream with a url

   TODO: cover the situation where this function is called while a video is being played
*/
template<class Renderer>
int GL3VideoPlayer<Renderer>::i_OpenStream(std::string url)
{
  if ( !open_urls.empty()  )
  {
    auto search = open_urls.find(url);
    std::cout << "already open! \n";
    if ( search != open_urls.end() ) return -1;
  }
  // the url was already opened

  status = "Opening " + url;
  if (!_init )
  {
    status = "OpenStream called before initialzation.";
    std::cout << status << std::endl;
    return -1;
  }

  if (avformat_open_input(&format_ctx, url.c_str(),
          NULL, NULL) != 0)
  {
    status = "avformat_open_input failed for " + url;
    std::cout << status << std::endl;
    return -1;
  }

  if (avformat_find_stream_info(format_ctx, NULL) < 0)
  {
    status = "avformat_find_stream_info failed";
    std::cout << status << std::endl;
    return -1;
  }

  for (size_t i = 0; i < format_ctx->nb_streams; i++)
  {   // TODO: ->codec is deprecated
      if (format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
          video_stream_index = i;
  }

  if (video_stream_index == -1)
  {
     status = "url opened but unable to find video stream";
    std::cout << status << std::endl;
    return -1;
  }
  else std::cout << "stream index set to " << video_stream_index << " " << codec_ctx << std::endl;
  avcodec_get_context_defaults3(codec_ctx, codec);

  avcodec_copy_context(codec_ctx, format_ctx->streams[video_stream_index]->codec);

  if (avcodec_open2(codec_ctx, codec, nullptr) < 0)
  {
    status =  "url opened but unable to find video stream";
    std::cout << status << std::endl;
    return -1;
  }

  img_convert_ctx = sws_getContext(
    codec_ctx->width,
    codec_ctx->height,
    codec_ctx->pix_fmt,
    codec_ctx->width,
    codec_ctx->height,
    AV_PIX_FMT_GRAY8,
    SWS_BICUBIC,
    nullptr, nullptr, nullptr);

  //TODO: avpicture_get_size is deprecated
  int picture_size = avpicture_get_size(
    AV_PIX_FMT_YUV420P,
    codec_ctx->width,
    codec_ctx->height);

  if (picture_size < 0)
  {
    status = "failed to get picture size";
    std::cout << status << std::endl;
    return -1;
  }
  int size2 = avpicture_get_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height);
  uint8_t* picture_buf2 = (uint8_t*)(av_malloc(size2));
  picture_buffer = (uint8_t*) (av_malloc(picture_size));
  if (picture_buffer == nullptr || picture_buf2 == nullptr)
  {
    status = "av_malloc failed for picture_buffer";
    std::cout << status << std::endl;
    return -1;
  }

  avpicture_fill(
    (AVPicture *) _rgb_frame,
    picture_buf2,
    AV_PIX_FMT_GRAY8,
    codec_ctx->width,
    codec_ctx->height);

  avpicture_fill(
    (AVPicture *) picture,
    picture_buffer,
    AV_PIX_FMT_YUV420P,
    codec_ctx->width,
    codec_ctx->height);

  av_init_packet(&packet);

  av_read_play(format_ctx);

  if (stream == nullptr)
  {    //create stream in file
      stream = avformat_new_stream(output_ctx,
              format_ctx->streams[video_stream_index]->codec->codec);
      avcodec_copy_context(stream->codec,
              format_ctx->streams[video_stream_index]->codec);
      stream->sample_aspect_ratio =
              format_ctx->streams[video_stream_index]->codec->sample_aspect_ratio;
  }

  _stream_open = true;
  status = "Stream open for " + url;
  open_urls.insert(url);
  return 1;
}

template<class Renderer>
int GL3VideoPlayer<Renderer>::i_RenderFrame(TimeSegment* tf)
{
  int val = 0;
  if (tf != nullptr) tf->Start();
  static size_t frame_count = 0;

  AVFrame _next_frame;
  if ( _rgb_frame_q.size_approx() > 1) return val;
  if (_rgb_frame_q.try_dequeue(_next_frame))
  {
    if (frame_count == 0) Renderer::RenderFrame_0(&_next_frame);
    else Renderer::RenderFrame_1(&_next_frame);
    frame_count++;
    val = 1;
  }
  if (tf != nullptr) tf->Stop();
  return val;
}

template<class Renderer>
int GL3VideoPlayer<Renderer>::i_ReadFrame(TimeSegment* tf)
{
  if (! (_init && _stream_open )) return -1;
  static size_t frame_count = 0;
  if (tf != nullptr) tf->Start();
  if (av_read_frame(format_ctx, &packet) >= 0)
  {
    int frame_finished = 0;
    packet.stream_index = stream->id;

    int result = avcodec_decode_video2(
      codec_ctx,
      picture,
      &frame_finished,
      &packet);

    if (frame_finished)
    {
      sws_scale(
        img_convert_ctx,
        picture->data,
        picture->linesize,
        0,
        codec_ctx->height,
        _rgb_frame->data,
        _rgb_frame->linesize);

      _rgb_frame_q.try_enqueue(*_rgb_frame);
    }
    frame_count++;
    av_free_packet(&packet);
    av_init_packet(&packet);
  }
  if (tf != nullptr) tf->Stop();
  return frame_count;
}

void GL3FrameRenderer_Texture2D::RenderFrame_1(AVFrame* frame)
{
  glActiveTexture(tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, frame->data[0]);
}

void GL3FrameRenderer_Texture2D::RenderFrame_0(AVFrame* frame)
{
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width,height,0,GL_RED,GL_UNSIGNED_BYTE, frame->data[0]);
}
