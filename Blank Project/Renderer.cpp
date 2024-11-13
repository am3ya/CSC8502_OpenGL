#include "Renderer.h"

#include "../nclgl/Light.h"
#include"../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include <algorithm>
const int POST_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"noise.png");

	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	//waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"grass.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"grassBump.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!earthTex || !earthBump || !cubeMap || !waterTex /* || !waterBump*/) {
		return;
	}
	setTextureRepeating(earthTex, true);
	setTextureRepeating(earthBump, true);
	setTextureRepeating(waterTex, true);
	//setTextureRepeating(waterBump, true);

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	processShader = new Shader("TexturedVertex.glsl", "processFrag.glsl");

	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess()
		|| !sceneShader->LoadSuccess() || !processShader->LoadSuccess()) {
		return;
	}

	treeMesh = Mesh::LoadFromMeshFile("tree-maple-low-poly-Anim.msh");
	treeAnim = new MeshAnimation("tree-maple-low-poly-Anim.anm");
	treeMaterial = new MeshMaterial("tree-maple-low-poly-Anim.mat");

	Vector3 heightMapSize = heightMap->GetHeightMapSize();

	camera = new Camera(-45.0f, 0.0f, heightMapSize * Vector3(0.5f, 5.0f, 0.5f));
	light = new Light(heightMapSize * Vector3(0.3f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x * 2.5);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;
}

Renderer::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
}

void Renderer::UpdateScene(float dt) {
	camera->updateCamera(dt);
	viewMatrix = camera->buildViewMatrix();
	waterRotate += dt * 2.0f; //2 degrees a second
	waterCycle += dt * 0.5f; //10 units a second
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawHeightMap();
	DrawWater();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightMap() {
	BindShader(lightShader);
	SetShaderLighting(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity(); //new
	textureMatrix.ToIdentity(); //new

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);
	//glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "waterBumpTex"), 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, waterBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightMapSize();

	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 2.0f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	UpdateShaderMatrices();
	//SetShaderLighting(*light) //No Lighting in this shader
	quad->Draw();
}

