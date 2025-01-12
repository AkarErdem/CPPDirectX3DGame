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
	/*MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame(0);
	}

	if (gResult == -1) {
		return -1;
	}

	return msg.wParam;*/

	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto exitCode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *exitCode;
		}
		// execute the game logic
		const auto dt = timer.Mark()  /*speed_factor*/;
		HandleInput(dt);
		DoFrame(dt);
	}
}

void App::HandleInput(float dt)
{
}

void App::DoFrame(float dt)
{
	static float elapsedTime = 0.0f; // Accumulated time
	elapsedTime += dt;              // Add the delta time

	// Update title
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(1) << std::fixed << elapsedTime << "s";
	window.SetTitle(oss.str());

	// Add sine wave to give some animated color
	const float c = sin(elapsedTime) / 2.0f + .5f;

	Graphics& gfx = window.Gfx();
	gfx.ClearBuffer(c, c, 1.0f); // White to blue

	// End graphics frame;
	gfx.EndFrame();
}
