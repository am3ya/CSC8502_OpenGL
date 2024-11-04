#include "Renderer.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/Camera.h"
#include <algorithm> //For std::sort

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, (Vector3(0, 100, 750.0f)));
	quad = Mesh::GenerateQuad();
	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");

	shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");

	texture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!shader->LoadSuccess() || !texture) {
		return;
	}

	root = new SceneNode();

	for (int i = 0; i < 5; ++i) {
		SceneNode* s = new SceneNode();
		s->setColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->setTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s->setModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->setBoundingRadius(100.0f);
		s->setMesh(quad);
		s->setTexture(texture);
		root->addChild(s);
	}

	root->addChild(new CubeRobot(cube));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

Renderer::~Renderer(void) {
	delete root;
	delete quad;
	delete camera;
	delete cube;
	delete shader;
	glDeleteTextures(1, &texture);
}

void Renderer::UpdateScene(float dt) {
	camera->updateCamera(dt);
	viewMatrix = camera->buildViewMatrix();
	frameFrustum.fromMatrix(projMatrix * viewMatrix);

	root->update(dt);
}

void Renderer::buildNodeLists(SceneNode* from) {
	if (frameFrustum.insideFrustum(*from)) {
		Vector3 dir = from->getWorldTransform().GetPositionVector() - camera->GetPosition();
		from->setCameraDistance(Vector3::Dot(dir, dir));

		if (from->getColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		buildNodeLists((*i));
	}
}

void Renderer::sortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::compareByCameraDistance); //Note the r
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::compareByCameraDistance);
}

void Renderer::drawNodes() {
	for (const auto& i : nodeList) {
		drawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		drawNode(i);
	}
}

void Renderer::drawNode(SceneNode* n) {
	if (n->getMesh()) {
		Matrix4 model = n->getWorldTransform() * Matrix4::Scale(n->getModelScale());

		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->getColour());

		texture = n->getTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), texture);

		n->Draw(*this);
	}
}

void Renderer::RenderScene() {
	buildNodeLists(root);
	sortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	drawNodes();

	clearNodeLists();
}

void Renderer::clearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}
