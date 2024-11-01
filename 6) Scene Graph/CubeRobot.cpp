#include "CubeRobot.h"

CubeRobot::CubeRobot(Mesh* cube) {
	//setMesh(cube); //Uncomment if you want a local origin marker

	SceneNode* body = new SceneNode(cube, Vector4(1, 0, 0, 1)); //Red
	body->setModelScale(Vector3(10, 15, 5));
	body->setTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	addChild(body);

	head = new SceneNode(cube, Vector4(0, 1, 0, 1)); //Green
	head->setModelScale(Vector3(5, 5, 5));
	head->setTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	body->addChild(head);

	leftArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	leftArm->setModelScale(Vector3(3, -18, 3));
	leftArm->setTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	body->addChild(leftArm);

	rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	rightArm->setModelScale(Vector3(3, -18, 3));
	rightArm->setTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	body->addChild(rightArm);

	SceneNode* leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	leftLeg->setModelScale(Vector3(3, -17.5, 3));
	leftLeg->setTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	body->addChild(leftLeg);

	SceneNode* rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	rightLeg->setModelScale(Vector3(3, -17.5, 3));
	rightLeg->setTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	body->addChild(rightLeg);
}

void CubeRobot::update(float dt) {
	transform = transform * Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));

	head->setTransform(head->getTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(0, 1, 0)));

	leftArm->setTransform(leftArm->getTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));

	rightArm->setTransform(rightArm->getTransform() * Matrix4::Rotation(30.0f * dt, Vector3(1, 0, 0)));

	SceneNode::update(dt);
}