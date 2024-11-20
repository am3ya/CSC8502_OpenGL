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
class Tree;

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
	void DrawTree();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* fadeShader;

	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Light* light2;
	Camera* camera;
	Mesh* tree1;
	Mesh* soldierMesh;
	MeshAnimation* soldierAnim;
	MeshMaterial* soldierMeshMat;

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void ApplyFadeEffect(float fadeFactor);
	void DrawNodes();
	void DrawNode(SceneNode* n);
	SceneNode* root;
	Frustum frameFrustum;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	Shader* shader;
	GLuint texture;

	GLuint cubeMap;
	GLuint apocalypseCubeMap;
	GLuint waterTex;
	GLuint treeTex;
	GLuint earthTex;
	GLuint earthBump;
	GLuint apocalypseTex;
	GLuint apocalypseBump;
	//GLuint waterBump;

	float waterRotate;
	float waterCycle;

	Mesh* treeMesh;
	Shader* treeShader;
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
	int currentFrame;
	float frameTime;
	float elapsedTime;
	bool fadingOut;
	float fadeFactor;
};