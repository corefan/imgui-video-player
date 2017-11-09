#ifndef __POINTSHADERS_H
#define __POINTSHADERS_H

#include <string>

const std::string SimpleVertexShader =
"#version 330 core\n"
"layout (location=0) in vec3 position;\n"
"\n"
"uniform mat4 transform;\n"
" void main() {\n"
" 	gl_Position = transform * vec4(position, 1.0);\n"
"}\n";

#endif