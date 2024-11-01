#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);


	void RenderScene() override;

	void toggleScissor();
	void toggleStencil();

protected:
	Shader* shader;
	Mesh* meshes[2];
	GLuint textures[2];

	bool usingScissor;
	bool usingStencil;
};
