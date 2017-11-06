#ifndef __imgui_impl_glfw_gl3_01_h
#define __imgui_impl_glfw_gl3_01_h

// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui
#include <functional>
#include <array>

#include <imgui.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

struct GLFWwindow;
using Bool3 = std::array<bool,3>;


class ImGui_ImplGlfwGL
{

public:
  ImGui_ImplGlfwGL(GLFWwindow* window, ImGuiIO* io, bool install_callbacks, std::function<void(ImDrawData*)> RenderDrawLists);
  ~ImGui_ImplGlfwGL();
  static const char* GetClipboardText(void* user_data);
  static void SetClipboardText(void* user_data, const char* text);
  static void MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/);
  static void ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset);
  static void KeyCallback(GLFWwindow*, int key, int, int action, int mods);
  static void CharCallback(GLFWwindow*, unsigned int c);
  bool CreateFontsTexture();
  bool CreateDeviceObjects();
  void InvalidateDeviceObjects();
  void Shutdown();
  void NewFrame();
  void RenderDrawLists(ImDrawData* draw_data);

  //
  // IMGUI_API bool        Init(GLFWwindow* window, bool install_callbacks);
  // IMGUI_API void        Shutdown();
  // IMGUI_API void        NewFrame();
  //
  // // Use if you want to reset your rendering device without losing ImGui state.
  // IMGUI_API void        InvalidateDeviceObjects();
  // IMGUI_API bool        CreateDeviceObjects();
  //
  // // GLFW callbacks (installed by default if you enable 'install_callbacks' during initialization)
  // // Provided here if you want to chain callbacks.
  // // You can also handle inputs yourself and use those as a reference.
  // IMGUI_API void        MouseButtonCallback(int button, int action, int mods);
  // IMGUI_API void        ScrollCallback(double xoffset, double yoffset);
  // IMGUI_API void        KeyCallback(int key, int scancode, int action, int mods);
  // IMGUI_API void        CharCallback(unsigned int c);

private:
  // Data
  GLFWwindow*  g_Window;
  static double       g_Time ;
  static Bool3         g_MousePressed;
  static float        g_MouseWheel;
  GLuint       g_FontTexture;
  int          g_ShaderHandle;
  int           g_VertHandle;
  int           g_FragHandle;
  int          g_AttribLocationTex;
  int         g_AttribLocationProjMtx;
  int          g_AttribLocationPosition;
  int           g_AttribLocationUV;
  int           g_AttribLocationColor;
  unsigned int g_VboHandle;
  unsigned int g_VaoHandle;
  unsigned int g_ElementsHandle;
  ImGuiIO& g_io;
  std::function<void(ImDrawData*)> g_RenderDrawListsFn;

};

#endif
