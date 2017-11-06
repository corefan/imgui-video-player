#ifndef __TESTGL_H
#define __TESTGL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <cstring>
#include <map>

#include "shaders/PassthroughVT.h"
#include "shaders/WobblyTextureFS.h"
#include "shaders/FragmentShadingRTT.h"
#include "shaders/VertexShadingRTT.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "controls.hpp"
#include "gl_state.h"

class framebuffer_test
{

  struct PackedVertex{
  	glm::vec3 position;
  	glm::vec2 uv;
  	glm::vec3 normal;
  	bool operator<(const PackedVertex that) const{
  		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
  	};
  };

public:
  void render();
  void computeMatricesFromInputs();
  GLuint texture() { return  _renderedTexture; }
  framebuffer_test(size_t w, size_t h, GLFWwindow* window) ;
  glm::mat4 getViewMatrix();
  glm::mat4 getProjectionMatrix();

private:
  bool getSimilarVertexIndex_fast(
  	PackedVertex & packed,
  	std::map<PackedVertex,unsigned short> & VertexToOutIndex,
  	unsigned short & result);

  void indexVBO(
  	std::vector<glm::vec3> & in_vertices,
  	std::vector<glm::vec2> & in_uvs,
  	std::vector<glm::vec3> & in_normals,
  	std::vector<unsigned short> & out_indices,
  	std::vector<glm::vec3> & out_vertices,
  	std::vector<glm::vec2> & out_uvs,
  	std::vector<glm::vec3> & out_normals );

  bool loadOBJ(
  	const std::string& path,
  	std::vector<glm::vec3> & out_vertices,
  	std::vector<glm::vec2> & out_uvs,
  	std::vector<glm::vec3> & out_normals);

  GLuint loadDDS(const std::string& imagepath);
  GLuint LoadShaders( const std::string&, const std::string&);
  bool _ready;
  GLFWwindow* _window;
  size_t _w;
  size_t _h;
  // Initial horizontal angle : toward -Z
  float _horizontalAngle;
  // Initial vertical angle : none
  float _verticalAngle;
  // Initial Field of View
  float _initialFoV;
  float _speed;
  // float _mouseSpeed;
  glm::vec3 _position;


  std::array<GLenum,1> _DrawBuffers;

  // The fullscreen quad's FBO
	const std::array<GLfloat,18> _g_quad_vertex_buffer_data = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};


  GLuint _quad_vertexbuffer;
  GLuint _quad_programID;
  GLuint _texID;
  GLuint _timeID;
  GLuint _VertexShaderID;
	GLuint _FragmentShaderID;
  GLuint _ProgramID;
  GLuint _MatrixID;
  GLuint _ViewMatrixID;
  GLuint _ModelMatrixID;
  GLuint _VertexArrayID;
  GLuint _FramebufferName;
  GLuint _renderedTexture;
  GLuint _depthrenderbuffer;
  GLuint _vertexbuffer;
  GLuint _uvbuffer;
  GLuint _normalbuffer;
  GLuint _elementbuffer;
  GLuint _LightID;
  GLuint _quad_ProgramID;
  GLuint _Texture;
  GLuint _TextureID;
  glm::mat4 _ViewMatrix;
  glm::mat4 _ProjectionMatrix;
  std::vector<unsigned short> indices;
  gl_state glState;
};

#endif
