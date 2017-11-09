#ifndef __3DSTUFF_H
#define __3DSTUFF_H
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "ViewMatrix.h"
#include "GLUtils.h"


class Points3d :  public GLUtils
{
public:
	Points3d();
	size_t size() { return vertices.size(); }
	GLuint RenderToTexture();
	void BindPoints();
	void Draw();

private:
	TransformationMatrix transform;
	std::vector<vec3f> vertices;
	std::vector<vec3f> colors;

// OpenGL
	GLuint _vertices_id;	
	GLuint _vertices_buf;
	GLuint _shaderProgram;
};
#endif