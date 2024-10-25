#include "../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("Vertex Transformation!", 800, 600, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	float scale = 100.0f;
	float rotation = 0.0f;
	Vector3 position(0, 0, -1500.0f);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1))
			renderer.switchToOrthographic();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2))
			renderer.switchToPerspective();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS))  ++scale;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS)) --scale;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))  ++rotation;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) --rotation;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))
			position.y -= 1.0f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
			position.y += 1.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
			position.x -= 1.0f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
			position.x += 1.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
			position.z -= 1.0f;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P))
			position.z += 1.0f;

		renderer.setRotation(rotation);
		renderer.setScale(scale);
		renderer.setPosition(position);
		renderer.RenderScene();
		renderer.SwapBuffers();
		renderer.updateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
	}

	return 0;
}