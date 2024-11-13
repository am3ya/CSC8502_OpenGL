#pragma once

#include"../nclgl/OGLRenderer.h"
#include"../nclgl/SceneNode.h"
#include"../nclgl/Frustum.h"

class Camera;
class Shader;
class HeightMap;
class MeshMaterial;
class SceneNode;
class MeshAnimation;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void DrawHeightMap();
	void DrawWater();
	void DrawSkybox();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;

	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Camera* camera;

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	SceneNode* root;
	Frustum frameFrustum;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	Shader* shader;
	GLuint texture;

	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	//GLuint waterBump;

	float waterRotate;
	float waterCycle;

	Mesh* treeMesh;
	Shader* meshShader;
	Shader* animMeshShader;
	MeshAnimation* treeAnim;
	MeshMaterial* treeMaterial;
	vector<GLuint> treeTextures;

	void DrawPostProcess();
	Shader* processShader;
	Shader* sceneShader;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	bool postProcess;
};