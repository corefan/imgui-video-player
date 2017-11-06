#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

class triangle_test
{

public:
  triangle_test(GLFWwindow* w) : g_window{w} { }
  triangle_test()
  {
    GLFWwindow *last_window =  	glfwGetCurrentContext();

      glfwSetErrorCallback(error_callback);
      if (!glfwInit())
        exit(EXIT_FAILURE);
      g_window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
      if (!g_window)
      {
          glfwTerminate();
          exit(EXIT_FAILURE);
      }
      glfwMakeContextCurrent(g_window);

      if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
      }
      glfwMakeContextCurrent(last_window);


  }

  void Render();
  static void error_callback(int error, const char* description);
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
  GLFWwindow* g_window;
};

void triangle_test::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void triangle_test::error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void triangle_test::Render()
{
  GLFWwindow *last_window =  	glfwGetCurrentContext();
  glfwMakeContextCurrent(g_window);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glewInit();
  // glfwSetKe  yCallback(g_window, key_callback);
  static size_t counter = 0;
  counter++;
  counter = counter % 60;
  float ratio;
  int width, height;
  glfwGetFramebufferSize(g_window, &width, &height);
  ratio = width / (float) height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.3f * (static_cast<float>(counter)/60.0f), 0.5f, 0.8f, 1.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
  glBegin(GL_TRIANGLES);
  glColor3f(1.f, 0.f, 0.f);
  glVertex3f(-0.6f, -0.4f, 0.f);
  glColor3f(0.f, 1.f, 0.f);
  glVertex3f(0.6f, -0.4f, 0.f);
  glColor3f(0.f, 0.f, 1.f);
  glVertex3f(0.f, 0.6f, 0.f);
  glEnd();
  glfwSwapBuffers(g_window);
  glfwPollEvents();

  glfwMakeContextCurrent(last_window);

}
