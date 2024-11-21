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
	void DrawSoldier();
	void SetShaderLighting(const Light& light1, const Light& light2);
	void OrbitCamera(float dt);
	void HandleInput();
	void DrawScene();
	void ApplyFadeEffect(float fadeFactor);
	void DrawPostProcess();
	void PresentScene();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* fadeShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* postquad;

	Light* light;
	Light* light2;
	Camera* camera;
	Camera* camera2;
	Mesh* tree1;
	Mesh* soldierMesh;
	MeshAnimation* soldierAnim;
	MeshMaterial* soldierMeshMat;
	vector<GLuint> soldierTextures;
	
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
	bool fadeChange;
	bool useApocalypseSkyBox;
	bool cameraOrbit;
};