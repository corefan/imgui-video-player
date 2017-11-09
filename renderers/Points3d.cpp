#include "Points3d.h"
#include "shaders/PointShaders.h"

Points3d::Points3d()
{
	glGenVertexArrays(1, &_vertices_id);
	glBindVertexArray(_vertices_id);
	glGenBuffers(1, &_vertices_buf);

	_shaderProgram = GLUtils::LoadShaders
}

void Points3d::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vertices_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
	glUseProgram(_shaderProgram);
}

void Points3d::BindPoints()
{
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_shaderTransform, 1, GL_FALSE, transform.data());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertices_buf);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(_real_vertices->size()),_real_vertices->data(), GL_DYNAMIC_DRAW);
			ErrorCheckValue = glGetError();
	glBindVertexArray(_vertices_buf);
}


// glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);