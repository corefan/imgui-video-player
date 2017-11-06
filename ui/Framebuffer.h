#ifndef __FRAMEBUFFER_H
#define __FRAMEBUFFER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>


class Framebuffer
{

public:
	Framebuffer(size_t w, size_t h);
	void Render();

private:
	GLuint LoadShaders(
		const std::string& ,
		const std::string& );

// width and height of framebuffer
	size_t _w, _h;
	std::vector<float> _vertices;

	GLuint _framebuffer_id;
	GLuint _framebuffer;
	GLuint _colorTex;
	GLuint _renderTargets[1];
	GLuint _shaderProgram;
	GLuint _vertices_id;
	GLuint _vertices_buf;

};

#endif