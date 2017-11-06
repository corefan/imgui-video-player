#include "Framebuffer.h"
#include <iostream>

#include "gl_state.h"

#include "shaders/PassthroughVT.h"
#include "shaders/FragmentShadingRTT.h"

Framebuffer::Framebuffer(size_t w, size_t h)
: _w{w} , _h{h}
{
	gl_state glState;
	glState.backup();

// vertices
	glGenVertexArrays(1, &_vertices_id);
	glBindVertexArray(_vertices_id);

	glGenBuffers(1, &_vertices_buf);
	glBindBuffer(GL_ARRAY_BUFFER, _vertices_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_vertices.size(), _vertices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);


// generate a framebuffer 
	glGenFramebuffers(1, &_framebuffer_id);	

// bind it as the target for rendering commands
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebuffer_id);

// set width, height, color depth
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, _w);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, _h);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 4);

	glGenRenderbuffers(1, &_framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _framebuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _w, _h);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _framebuffer);

// create a RGBA color texture
	glGenTextures(1, &_colorTex);
	glBindTexture(GL_TEXTURE_2D, _colorTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
				_w, _h, 
				0, GL_RGBA, GL_UNSIGNED_BYTE,
				nullptr);

	glFramebufferTexture2D(_framebuffer_id,GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,_colorTex, 0);

	GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (e != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "There is a problem with the FBO" << std::endl;
	}

// a framebuffer can render to multiple textures. only one texture in this case.
// _renderTargets could hold multiple references to previous calls to glFramebufferTexture 
	_renderTargets[0] = GL_COLOR_ATTACHMENT0;

//unbind the framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

// shaders
	_shaderProgram = LoadShaders(SimpleVertexShaderCode, SimpleFragmentShaderCode);

	glState.restore();
}

void Framebuffer::Render()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebuffer_id);
	glDrawBuffers(1, _renderTargets);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(_shaderProgram);

	glBindVertexArray(_vertices_id);
	glDrawArrays(GL_POINTS, 0, 12);

}

GLuint Framebuffer::LoadShaders(
	const std::string& VertexShaderCode,
	const std::string& FragmentShaderCode)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;
	// Compile Vertex Shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
  {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
  {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}
	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}