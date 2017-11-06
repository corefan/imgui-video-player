#ifndef __GL_STATE_H
#define __GL_STATE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class gl_state
{
public:
  void backup();
  void restore();
  gl_state() : has_state{false} { }

private:
  bool has_state;
  GLenum last_active_texture;
  GLint last_program;
  GLint last_texture;
  GLint last_sampler;
  GLint last_array_buffer;
  GLint last_drawFboId;
  GLint last_readFboId;
  GLint last_element_array_buffer;
  GLint last_vertex_array;
  GLint last_polygon_mode[2];
  GLint last_viewport[4];
  GLint last_scissor_box[4];
  GLenum last_blend_src_rgb;
  GLenum last_blend_dst_rgb;
  GLenum last_blend_src_alpha;
  GLenum last_blend_dst_alpha;
  GLenum last_blend_equation_rgb;
  GLenum last_blend_equation_alpha;
  GLboolean last_enable_blend;
  GLboolean last_enable_cull_face;
  GLboolean last_enable_depth_test;
  GLboolean last_enable_scissor_test;
};

#endif
