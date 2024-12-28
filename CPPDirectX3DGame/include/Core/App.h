#pragma once
#include "Window/Window.h"

class App
{
public:
	App();
	~App();
	// master frame / message loop
	int Start();
private:
	void DoFrame(float dt);
private:
	Window window;
};