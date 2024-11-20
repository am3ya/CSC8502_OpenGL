#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void setTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& getTransform() const { return transform; }
	Matrix4 getWorldTransform() const { return worldTransform; }

	Vector4 getColour() const { return colour; }
	void setColour(Vector4 c) { colour = c; }

	Vector3 getModelScale() const { return modelScale; }
	void setModelScale(Vector3 s) { modelScale = s; }

	Mesh* getMesh() const { return mesh; }
	void setMesh(Mesh* m) { mesh = m; }

	float getBoundingRadius() const { return boundingRadius; }
	void setBoundingRadius(float f) { boundingRadius = f; }

	float getCameraDistance() const { return distanceFromCamera; }
	void setCameraDistance(float f) { distanceFromCamera = f; }

	void setTexture(GLuint tex) { texture = tex; }
	GLuint getTexture() const { return texture; }

	static bool compareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	void addChild(SceneNode* s);

	virtual void update(float dt);
	virtual void Draw(const OGLRenderer& r);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	//Added myself
	Shader* getShader() {
		return shader;
	}

protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	float distanceFromCamera;
	float boundingRadius;
	GLuint texture;
	//Addded myself
	Shader* shader;
	std::vector<SceneNode*> children;
};