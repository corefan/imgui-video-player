#include "Framebuffer.h"
#include <iostream>
#include <math.h>

#include "gl_state.h"

#include "shaders/PassthroughVT.h"
#include "shaders/FragmentShadingRTT.h"

void Framebuffer::setViewMatrix()
{
	Eigen::Matrix4f A;
	Eigen::Matrix4f B = Eigen::Matrix4f::Identity();
	A.row(0) << _cameraRight(0), _cameraRight(1), _cameraRight(2) , 0.0f;
	A.row(1) << _cameraUp(0), _cameraUp(1), _cameraUp(2) , 0.0f;
	A.row(2) << _cameraDirection(0), _cameraDirection(1), _cameraDirection(2), 0.0f;
	A.row(3) << 0.0f, 0.0f, 0.0f, 1.0f;
	B.col(3) << -1*_camera(0), -1*_camera(1), -1*_camera(2) , 1.0f;
	_view = A*B;
}

void Framebuffer::setProjectionMatrix()
{
	float tha = 1.0/tan(M_PI * (_fov/360.0f));
	float rat = static_cast<float>(_w) / static_cast<float>(_h);
	_projection.row(0) << tha , 0.0f, 0.0f , 0.0f;
	_projection.row(1) << 0.0f , tha/rat , 0.0f, 0.0f;
	_projection.row(2) << 0.0f, 0.0f , -1.0f*(_near + _far)/(_far - _near), -2.0f * _far * _near / ( _far - _near );
	_projection.row(3) << 0.0f, 0.0f, -1.0f, 0.0f;
}

void Framebuffer::setTransformMatrix()
{
	_transform =  _projection;
}

Framebuffer::Framebuffer(size_t w, size_t h)
: _w{w} , _h{h}, _camera{0.0f, 0.0f, 2.0f}, _cameraTarget{0.0f, 0.0f, 0.0f},
	_up{0.0f,1.0f,0.0f}, _near{0.0001f}, _far{100.0f}, _fov{160.0f}
{
	_cameraDirection = (_camera - _cameraTarget).normalized();
	_cameraRight = _up.cross(_cameraDirection);
	_cameraUp = _cameraDirection.cross(_cameraRight);
	setViewMatrix();
	setProjectionMatrix();
	setTransformMatrix();

	_vertices = Eigen::VectorXf::Random(3000);
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
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 3);

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

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, _colorTex, 0);

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
	GLenum ErrorCheckValue = glGetError();

	_shaderTransform = glGetUniformLocation(_shaderProgram, "transform");

// done

  if (ErrorCheckValue != GL_NO_ERROR)
  	std::cout << "ERROR: Could not create a VBO " << gluErrorString(ErrorCheckValue) << std::endl;
 
	glState.restore();
}

void Framebuffer::Render()
{
	GLenum ErrorCheckValue = glGetError();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebuffer);

	glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
	glPointSize(4.0);
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1,  attachments);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,_w,_h);

	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_shaderTransform, 1, GL_FALSE, _transform.data());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertices_buf);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_vertices.size(), _vertices.data(), GL_STATIC_DRAW);
			ErrorCheckValue = glGetError();

	glBindVertexArray(_vertices_buf);
	glDrawArrays(GL_POINTS, 0, _vertices.size()/3 );

  if (ErrorCheckValue != GL_NO_ERROR)
  	std::cout << "Render ERROR:  " << gluErrorString(ErrorCheckValue) << std::endl;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

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