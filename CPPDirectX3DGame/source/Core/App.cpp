#include "Core/App.h"
#include "Window/Window.h"
#include "Input/Mouse.h"
#include <sstream>

App::App()
	: window(800, 300, "CPP Direct3D11 Game")
{
}

App::~App()
{
}

int App::Start()
{
	MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame(0);
	}

	if (gResult == -1) {
		return -1;
	}

	return msg.wParam;
}

void App::DoFrame(float dt)
{
	while (!window.mouse.IsEmpty()) {
		const auto e = *window.mouse.Read();
		if (e.GetType() == Mouse::Event::Type::Move) {
			std::ostringstream buffer;
			buffer << "Mouse position: (" << e.GetPosX() << "," << e.GetPosY() << ")";
			window.SetTitle(buffer.str());
		}
	}

	if (window.keyboard.IsKeyPressed(VK_SPACE)) {
		MessageBox(nullptr, "Hello!", "Space key was pressed", MB_OK | MB_ICONEXCLAMATION);
	}
}
