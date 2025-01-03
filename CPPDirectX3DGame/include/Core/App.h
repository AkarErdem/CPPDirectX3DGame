#pragma once
#include "Window/Window.h"
#include "Time/OTimer.h"

class App
{
public:
	App();
	~App();
	// master frame / message loop
	int Start();
private:
	void HandleInput(float dt);
	void DoFrame(float dt);
private:
	Window window;
	OTimer timer;
};