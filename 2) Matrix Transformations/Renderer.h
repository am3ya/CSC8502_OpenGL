#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void switchToPerspective();
	void switchToOrthographic();

	inline void setScale(float s) { scale = s; }
	inline void setRotation(float r) { rotation = r; }
	inline void setPosition(Vector3 p) { position = p; }

	virtual void updateScene(float dt);

protected:
	Mesh* triangle;
	Shader* matrixShader;
	Camera* camera;
	float scale;
	float rotation;
	Vector3 position;
};
