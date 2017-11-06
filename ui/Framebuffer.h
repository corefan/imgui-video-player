#ifndef __FRAMEBUFFER_H
#define __FRAMEBUFFER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>


class Framebuffer
{

public:
	Framebuffer(size_t w, size_t h);
	void Render();
	GLuint getTexture() { return _colorTex; }

private:
	void setViewMatrix();
	void setProjectionMatrix();
	void setTransformMatrix();
	GLuint LoadShaders(
		const std::string& ,
		const std::string& );

// width and height of framebuffer
	size_t _w, _h;
// vertices
	Eigen::VectorXf _vertices;

	Eigen::Vector3f _camera;
	Eigen::Vector3f _cameraTarget;
	Eigen::Vector3f _cameraDirection;
	Eigen::Vector3f _up;
	Eigen::Vector3f _cameraRight;
	Eigen::Vector3f _cameraUp;
	Eigen::Matrix4f _view;
	Eigen::Matrix4f _projection;
	Eigen::Matrix4f _transform;

	float _near;
	float _far;
	float _fov;

	GLuint _framebuffer_id;
	GLuint _framebuffer;
	GLuint _colorTex;
	GLuint _renderTargets[1];
	GLuint _shaderProgram;
	GLuint _vertices_id;
	GLuint _vertices_buf;

	GLuint _shaderTransform;

};

#endif