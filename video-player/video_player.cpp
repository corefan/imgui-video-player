
#define OUTPUT_FORMAT AV_PIX_FMT_GRAY8


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
GL3VideoPlayer<Renderer>::GL3VideoPlayer(size_t w, size_t h,  GLFWwindow* window)
:  Renderer(w,h) , format_ctx{nullptr} ,
   codec_ctx{nullptr} ,
   _stream_open{false} ,
   video_stream_index{-1} ,
   picture{nullptr} , picture_buffer{nullptr} ,
   stream{nullptr} , output_ctx{nullptr} ,
   codec{nullptr} , output_width{640} ,
   output_height{480}, output_channels{1},
   test_framebuffer{640,480}
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

template<class Renderer>
GL3VideoPlayer<Renderer>::~GL3VideoPlayer()
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

// TODO: fix this so it will set the output channels correctly
  img_convert_ctx = sws_getContext(
    codec_ctx->width,
    codec_ctx->height,
    codec_ctx->pix_fmt,
    output_width,
    output_height,
    OUTPUT_FORMAT,
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
  int size2 = avpicture_get_size(OUTPUT_FORMAT, codec_ctx->width, codec_ctx->height);
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
    OUTPUT_FORMAT,
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

// get an AVFrame from the queue and send it to the renderer
//
template<class Renderer>
int GL3VideoPlayer<Renderer>::i_RenderFrame(TimeSegment* tf)
{
  int val = 0;
  if (tf != nullptr) tf->Start();
  static size_t frame_count = 0;

  AVFrame _next_frame;
 
  mser_cpu_extractor mser1;
  Eigen::VectorXf region_centers;

  if ( _rgb_frame_q.size_approx() < 1) return val;
  bool result = _rgb_frame_q.try_dequeue(_next_frame);
  if ( result )
  {
    if ((frame_count % 10 ) == 0 )
    {
      size_t ncenters = mser1.Run(_next_frame.data[0], output_width, output_height);
      if (ncenters)
      {
        mser1.RegionsFirstMomentsToXYZ(region_centers);
        test_framebuffer.SetVertices(&region_centers);
        test_framebuffer.Render();
        std::cout << "video_player found " << ncenters << "MSER regions" << std::endl;

      }
	  }
 
		Renderer::RenderFrame_1_red(&_next_frame);

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



void GL3FrameRenderer_Texture2D::RenderFrame_1_red(AVFrame* frame)
{
  glActiveTexture(tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, frame->data[0]);

}

void GL3FrameRenderer_Texture2D::RenderFrame_2_red(uint8_t* frame)
{
  glActiveTexture(mers_tex);
  glBindTexture(GL_TEXTURE_2D, mers_tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, frame);
}

void GL3FrameRenderer_Texture2D::RenderFrame_0_red(AVFrame* frame)
{
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width,height,0,GL_RED,GL_UNSIGNED_BYTE, frame->data[0]);
}

