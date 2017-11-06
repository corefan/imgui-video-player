// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "video_player.hpp"
#define DEFAULT_URL "http://192.168.43.1:8080/video"

#include "Framebuffer.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

const std::string video_url{DEFAULT_URL};

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Time the video frame read and render
    TimeSegment timer_read_frame;
    TimeSegment timer_render_frame;
    TimeSegment timer_main_loop;


    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* gl_window_ctx = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(gl_window_ctx);
    glfwSwapInterval(1); // Enable vsync
    if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      return 1;
    }
    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(gl_window_ctx, true);

    // triangle_test test_triangle;


    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    bool show_test_window = true;
    bool show_video_window = true;
    bool main_window_closable = false;
    std::string show_video_msg = "Show Video";
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec2 workAreaSize = ImVec2(400,400);
    GLuint tex;
    glGenTextures(1, &tex);

    GL3VideoPlayer<GL3FrameRenderer_Texture2D> video1{640,480,gl_window_ctx};
    // GL3VideoPlayer video2{640,480,gl_window_ctx};
    std::future<int> open_video;
    std::future<int> render_frame;
    open_video = video1.t_OpenStream(video_url);

    // std::future<int> open_video_2;
    // open_video_2 = video2.t_OpenStream(video_url);
    // Main loop

    Framebuffer test_framebuffer(640,480);

    std::cout << "here we are" << std::endl;
    while (!glfwWindowShouldClose(gl_window_ctx))
    {

      long read_frame_start;
      long read_frame_stop;
      long render_frame_start;
      long render_frame_stop;
      long end = render_frame_stop;
      float endf;
      float rf0;
      float rf1;
      float df0;
      float df1;

      glfwPollEvents();
      ImGui_ImplGlfwGL3_NewFrame();

        if (show_video_window && video1.StreamIsOpen() )
        {
          // if the last call to t_ReadFrame has finished, call it again and render a frame
          if ( std::future_status::ready == open_video.wait_for(std::chrono::seconds(0)) )
          {
            timer_main_loop.Start();

            // i_RenderFrame should find the last Frame from t_ReadFrame waiting for it in the queue
            // TODO: reusing the future from open_video. sloppy
            open_video = video1.t_ReadFrame(&timer_read_frame);
            video1.i_RenderFrame(&timer_render_frame);

            read_frame_start =  std::chrono::duration_cast<Interval>( timer_read_frame.t0 - timer_main_loop.t0 ).count();
            read_frame_stop =  std::chrono::duration_cast<Interval>( timer_read_frame.t1 - timer_main_loop.t0 ).count();
            render_frame_start = std::chrono::duration_cast<Interval>( timer_render_frame.t0 - timer_main_loop.t0 ).count();
            render_frame_stop =  std::chrono::duration_cast<Interval>( timer_render_frame.t1 - timer_main_loop.t0 ).count();
            end = render_frame_stop > read_frame_stop ? render_frame_stop : read_frame_stop;
            endf = static_cast<float>(end);
            rf0 = static_cast<float>(read_frame_start) / endf;
            rf1 = static_cast<float>(read_frame_stop) / endf;
            df0 = static_cast<float>(render_frame_start) / endf;
            df1 = static_cast<float>(render_frame_stop) / endf;

          }
        }
        show_video_msg = show_video_window ? "Hide Video" : "Show Video" ;

        // 1. Show a simple gl_window_ctx
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a gl_window_ctx automatically called "Debug"
        {
          ImGui::Begin("Video");
          if (ImGui::Button(show_video_msg.c_str()))
          {
            show_video_window ^= 1;
          }
          static char buf[1024] = DEFAULT_URL;
          ImGui::InputText("Url", buf, IM_ARRAYSIZE(buf));
          // ImGui::ProgressBar(rf1, ImVec2(0.f,0.f), "read", rf0);
          // ImGui::ProgressBar(df1, ImVec2(0.f,0.f), "render", df0);
          ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
          ImGui::End();
        }

        // 2. Show another simple gl_window_ctx, this time using an explicit Begin/End pair
        if (show_video_window)
        {
          GLuint video_tex = video1.GetTextureId();
          ImGui::Begin("Another gl_window_ctx", &show_video_window);
          ImGui::Text(video1.GetStatusString());
          ImGui::Image((void *)video_tex, ImVec2(640, 480), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
          ImGui::End();
        }

        if (show_video_window)
        {



          // GLuint video_tex  = video1.GetTextureId_mers();
          ImGui::Begin("MSER", &show_video_window);

          test_framebuffer.Render();
          // ImGui::Text(video1.GetStatusString());
          ImGui::Image((void *)test_framebuffer.getTexture(), ImVec2(640, 480), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));

          ImGui::End();
        }

        // 3. Show the ImGui test gl_window_ctx. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
          ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
          ImGui::ShowTestWindow(&show_test_window);
        }

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(gl_window_ctx, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(gl_window_ctx);
        // test_triangle.Render();

    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();


    return 0;
}
