#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void updateTextureMatrix(float value);
	void toggleRepeating();
	void toggleFiltering();

protected:
	Shader* shader;
	Mesh* triangle;
	GLuint texture;
	bool filtering;
	bool repeating;
};
