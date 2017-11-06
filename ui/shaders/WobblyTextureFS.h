#ifndef __WobblyTextureFS_H
#define __WobblyTextureFS_H

const std::string WobblyTextureFS =
"#version 330 core\n"
"\n"
"in vec2 UV;\n"
"\n"
"out vec3 color;\n"
"\n"
"uniform sampler2D renderedTexture;\n"
"uniform float time;\n"
"\n"
"void main(){\n"
"	color = texture( renderedTexture, UV + 0.005*vec2( sin(time+1024.0*UV.x),cos(time+768.0*UV.y)) ).xyz ;\n"
"}\n";

#endif
