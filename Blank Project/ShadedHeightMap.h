#pragma once
#include "ShadedSceneNode.h"

class ShadedHeightMap :
	public ShadedSceneNode
{

public:
	ShadedHeightMap(Shader* shader, GLuint tex, Mesh* mesh, GLuint sandBump, GLuint grassTex, GLuint grassBump, GLuint rockTex, GLuint rockBump);
	void Draw(const OGLRenderer& r);
protected:
	GLuint sandBump;
	GLuint grassTex;
	GLuint grassBump;
	GLuint rockTex;
	GLuint rockBump;
};
