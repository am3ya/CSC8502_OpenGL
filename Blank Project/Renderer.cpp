#include "Renderer.h"

#include "../nclgl/Light.h"
#include"../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/Window.h"
//#include "HeightMap.h"
#include <algorithm>
const int POST_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	cameraOrbit = false;
	fadeChange = false;
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"noise.png");

	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	treeTex = SOIL_load_OGL_texture(TEXTUREDIR"tree-maple-D.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	//waterBump = SOIL_load_OGL_texture(TEXTU REDIR"waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"grass.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"grassBump.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	apocalypseTex = SOIL_load_OGL_texture(TEXTUREDIR"rock.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	apocalypseBump = SOIL_load_OGL_texture(TEXTUREDIR"rockBump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	apocalypseCubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"hellLeft.jpg", TEXTUREDIR"hellRight.jpg",
		TEXTUREDIR"hellTop.jpg", TEXTUREDIR"hellBottom.jpg",
		TEXTUREDIR"hellFront.jpg", TEXTUREDIR"hellBack.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!earthTex || !earthBump || !cubeMap || !waterTex /* || !waterBump*/ || !apocalypseTex || !apocalypseBump || !apocalypseCubeMap) {
		return;
	}
	setTextureRepeating(earthTex, true);
	setTextureRepeating(earthBump, true);
	setTextureRepeating(waterTex, true);
	setTextureRepeating(apocalypseTex, true);
	setTextureRepeating(apocalypseBump, true);
	//setTextureRepeating(waterBump, true);

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	processShader = new Shader("TexturedVertex.glsl", "processFrag.glsl");
	treeShader = new Shader("SkinningVertex.glsl", "TexturedFragment.glsl");
	fadeShader = new Shader("FadeVertex.glsl", "FadeFragment.glsl");

	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess()
		|| !sceneShader->LoadSuccess() || !processShader->LoadSuccess() || !treeShader->LoadSuccess()
		 || !fadeShader->LoadSuccess()) {
		return;
	}

	tree1 = Mesh::LoadFromMeshFile("tree-maple-low-poly-Anim.msh");
	treeAnim = new MeshAnimation("tree-maple-low-poly-Anim.anm");
	treeMaterial = new MeshMaterial("tree-maple-low-poly-Anim.mat");

	soldierMesh = Mesh::LoadFromMeshFile("Role_T.msh");
	soldierAnim = new MeshAnimation("Role_T.anm");
	soldierMeshMat = new MeshMaterial("Role_T.mat");

	for (int i = 0; i < soldierMesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = soldierMeshMat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		soldierTextures.emplace_back(texID);
	}

	/*for (int i = 0; i < tree1->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = treeMaterial->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		treeTextures.emplace_back(texID);
	}*/

	Vector3 heightMapSize = heightMap->GetHeightMapSize();

	Vector3 treePosition = Vector3((heightMapSize.x/0.5f), (heightMapSize.y/0.5f), (heightMapSize.z/0.5f)); // Tree's position in world space
	float treeScale = 100.0f; // Scale factor for the tree
	float treeRotation = 45.0f; // Rotation angle in degrees

	Matrix4 modelMatrix = Matrix4::Translation(treePosition) *
		Matrix4::Rotation(treeRotation, Vector3(0, 1, 0)) *
		Matrix4::Scale(Vector3(treeScale, treeScale, treeScale));

	camera = new Camera(0.0f, 27.5f, heightMapSize * Vector3(1.0f, 2.0f, 1.0f));
	Vector3 camera2Pos = heightMapSize * Vector3(0.3f, 0.5f, 0.5f);
	camera2Pos.y += 2000.0f;
	camera2 = new Camera(-90.0f, 0.0f, camera2Pos);
	light = new Light(heightMapSize * Vector3(0.3f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x * 3.0);
	light2 = new Light(heightMapSize * Vector3(0.7f, 1.2f, 0.5f), Vector4(0.6f, 0.2f, 0.4f, 1), heightMapSize.x * 3.0);
	//light2 = new Light(heightMapSize * Vector3(0.7f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x * 3.0);
	//tree1 = new Tree(lightShader, treeMesh, treeAnim, treeMaterial, Vector3(2800.0f, 350.0f, 2800.0f), 75.0f, 30.0f, false);

	//projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	//root = new SceneNode();

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;
	elapsedTime = 0.0f; // Start the timer at 0
	fadingOut = false;  // Start without fading
	fadeFactor = 0.0f;  // Fully visible
	currentFrame = 0;
	frameTime = 0.0f;
}

Renderer::~Renderer(void) {
	delete camera;
	delete camera2;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete treeShader;
	delete processShader;
	delete sceneShader;
	delete light;
	delete light2;
	delete treeAnim;
	delete treeMaterial;
	delete soldierAnim;
	delete soldierMeshMat;
	delete fadeShader;

	glDeleteTextures(1, &earthTex);
	glDeleteTextures(1, &earthBump);
	glDeleteTextures(1, &cubeMap);
	glDeleteTextures(1, &waterTex);
	glDeleteTextures(1, &treeTex);
	glDeleteTextures(1, &apocalypseTex);
	glDeleteTextures(1, &apocalypseBump);
	glDeleteTextures(1, &apocalypseCubeMap);

}

void Renderer::UpdateScene(float dt) {
	HandleInput();
	elapsedTime += dt;

	if (elapsedTime >= 10.0f && elapsedTime < 15.0f) { // Fade to black over 2 seconds
		fadingOut = true;
		fadeFactor = (elapsedTime - 10.0f) / 2.0f; // Linear fade from 0.0 to 1.0
	}
	else if (elapsedTime >= 15.0f && elapsedTime < 18.5f) { // Fade back to render
		fadingOut = false;
		fadeChange = true;
		fadeFactor = 1.0f - ((elapsedTime - 15.0f) / 2.0f); // Linear fade from 1.0 to 0.0
	}
	else if (elapsedTime >= 18.5f) { // Reset to fully visible
		fadeFactor = 0.0f;
	}

	if (cameraOrbit) {
		OrbitCamera(dt);
	}
	else {
		camera->updateCamera(dt);
	}
	//camera->updateCamera(dt);
	viewMatrix = camera->buildViewMatrix();
	waterRotate += dt * 2.0f; //2 degrees a second
	waterCycle += dt * 0.5f; //10 units a second

	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % soldierAnim->GetFrameCount();
		frameTime += 1.0f / soldierAnim->GetFrameRate();
	}

}

void Renderer::RenderScene() {
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//camera->updateCamera();
	viewMatrix = camera->buildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	DrawScene();
	
	int miniWidth = width / 6;
	int miniHeight = height / 6;
	int miniX = width - miniWidth - 10;
	int miniY = height - miniHeight - 10;

	glViewport(miniX, miniY, miniWidth, miniHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
	//camera2->updateCamera();
	viewMatrix = camera2->buildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)miniWidth / (float)miniHeight, 45.0f);
	DrawScene();
	
	
	
	
	
	
	
	
	
	
	
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//DrawSkybox();
	//DrawHeightMap();
	//DrawWater();
	//DrawTree();

	//if (fadeFactor > 0.0f) {
	//	ApplyFadeEffect(fadeFactor);
	//}

	//if (fadeFactor == 1.0f) {
		//DrawSkybox();
		//DrawHeightMap();
		//DrawWater();
		//DrawTree();
	//}
}

void Renderer::DrawScene() {
	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawTree();

	if (fadeFactor > 0.0f) {
		ApplyFadeEffect(fadeFactor);
	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	if (fadeChange) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, apocalypseCubeMap);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	}

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightMap() {
	BindShader(lightShader);
	SetShaderLighting(*light, *light2);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	if (!fadeChange) {
		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthTex);

		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, earthBump);
	}
	else {
		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, apocalypseTex);

		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, apocalypseBump);
	}
	//glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, earthTex);

	//glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity(); //new
	textureMatrix.ToIdentity(); //new

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawWater() {
	if (fadeChange) {
		return;
	}
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
	//glBindTexture(GL_TEXTURE_CUBE_MAP, apocalypseCubeMap);

	Vector3 hSize = heightMap->GetHeightMapSize();

	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 2.0f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	UpdateShaderMatrices();
	//SetShaderLighting(*light) //No Lighting in this shader
	quad->Draw();
}

/*void Renderer::DrawTree() {
	BindShader(lightShader); // Use the tree shader

	Vector3 heightMapSize = heightMap->GetHeightMapSize();

	Vector3 treePosition = Vector3((heightMapSize.x / 0.5f), (heightMapSize.y / 0.5f), (heightMapSize.z / 0.5f)); // Tree's position in world space
	float treeScale = 100.0f; // Scale factor for the tree
	float treeRotation = 45.0f; // Rotation angle in degrees

	Matrix4 modelMatrix = Matrix4::Translation(treePosition) *
		Matrix4::Rotation(treeRotation, Vector3(0, 1, 0)) *
		Matrix4::Scale(Vector3(treeScale, treeScale, treeScale));         // Scale

	UpdateShaderMatrices();

	glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);

	// Pass view and projection matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);

	// Pass camera position (for lighting, if required)
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	// Bind and render the tree mesh
	for (int i = 0; i < tree1->GetSubMeshCount(); ++i) {
		// Load texture for each submesh
		const MeshMaterialEntry* matEntry = treeMaterial->GetMaterialForLayer(i);
		const std::string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);

		std::string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);

		// Render the submesh
		tree1->DrawSubMesh(i);
	}
}*/


void Renderer::DrawTree() {
	if (fadeChange) {
		return;
	}
	BindShader(lightShader); // Use the correct shader
	SetShaderLighting(*light, *light2);

	Vector3 heightMapSize = heightMap->GetHeightMapSize();

	// Set up the model matrix for the tree's position, rotation, and scale
	Matrix4 modelMatrix = Matrix4::Translation(heightMapSize * Vector3(0.3f, 0.5f, 0.5f)/*Vector3(1000.0f, 50.0f, 800.0f)*/) *  // Position
		Matrix4::Rotation(0.0f, Vector3(0, 1, 0)) *             // Rotation
		Matrix4::Scale(Vector3(100.0f, 100.0f, 100.0f));        // Scale
	glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);

	// Pass view and projection matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);

	// Pass the light properties
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "lightPos"), 1, (float*)&light->GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader->GetProgram(), "lightColour"), 1, (float*)&light->GetColour());
	glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "lightRadius"), light->GetRadius());

	// Pass the camera position
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	// Bind the texture
	glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
	glBindTexture(GL_TEXTURE_2D, treeTex); // Bind the texture you loaded in the constructor
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0); // Set uniform to use texture unit 0

	// Draw the tree mesh
	for (int i = 0; i < tree1->GetSubMeshCount(); ++i) {
		tree1->DrawSubMesh(i);
	}
}

void Renderer::ApplyFadeEffect(float fadeFactor) {
	BindShader(fadeShader);

	glUniform1f(glGetUniformLocation(fadeShader->GetProgram(), "fadeFactor"), fadeFactor);

	glDisable(GL_DEPTH_TEST);
	quad->Draw();
	glEnable(GL_DEPTH_TEST);
}

void Renderer::SetShaderLighting(const Light& light1, const Light& light2) {
	GLuint program = lightShader->GetProgram();

	// Light 1
	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, (float*)&light1.GetPosition());
	glUniform4fv(glGetUniformLocation(program, "lightColour"), 1, (float*)&light1.GetColour());
	glUniform1f(glGetUniformLocation(program, "lightRadius"), light1.GetRadius());

	// Light 2
	glUniform3fv(glGetUniformLocation(program, "light2Pos"), 1, (float*)&light2.GetPosition());
	glUniform4fv(glGetUniformLocation(program, "light2Colour"), 1, (float*)&light2.GetColour());
	glUniform1f(glGetUniformLocation(program, "light2Radius"), light2.GetRadius());
}

void Renderer::HandleInput() {
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C)) {
		cameraOrbit = !cameraOrbit;
	}
}

void Renderer::OrbitCamera(float dt) {
	static float angle = 0.0f; // Keep track of the angle around the orbit
	angle -= (dt * 20.0f); // Adjust speed (degrees per second)

	// Convert angle to radians for trigonometric functions
	float radians = angle * (PI / 180.0f);

	// Define the radius of the orbit and the center of the scene
	float orbitRadius = 2800.0f; // Adjust to fit your scene
	Vector3 orbitCenter = heightMap->GetHeightMapSize() * 0.5f; // Center of the heightmap

	// Compute the camera's new position
	float x = orbitCenter.x + orbitRadius * cos(radians);
	float z = orbitCenter.z + orbitRadius * sin(radians);

	// Keep the camera at a fixed height (y-axis)
	float y = orbitCenter.y + 650.0f; // Adjust for your scene

	// Set the camera's position and make it look at the orbit center
	camera->SetPosition(Vector3(x, y, z));

	// Calculate the yaw angle to make the camera face the orbit center
	float dx = orbitCenter.x - x; // Direction vector (x component)
	float dz = orbitCenter.z - z; // Direction vector (z component)
	float yaw = atan2(dz, dx) * (180.0f / PI); // Convert radians to degrees

	//if (yaw < 0) {
	//	yaw += 360.0f;
	//}
	camera->setYaw(yaw);
	//camera->setYaw(-angle); // Update yaw to keep the camera facing the center
	camera->setPitch(-12.5f); // Keep the pitch level
}

void Renderer::DrawSoldier() {
	BindShader(treeShader);

	Vector3 heightMapSize = heightMap->GetHeightMapSize();



}
