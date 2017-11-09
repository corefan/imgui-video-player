#ifndef __GLUTILS_H
#define __GLUTILS_H

#include <string>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLUtils
{
public:
	GLuint LoadShaders(const std::string&, const std::string& );

};

#endif