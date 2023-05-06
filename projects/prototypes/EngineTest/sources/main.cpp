#include "Engine.h"

class Sandbox : public testEngine::Application
{
public:
	Sandbox()
	{

	}
	~Sandbox()
	{

	}
};

int main()
{
	Sandbox* sandbox = new Sandbox();
	sandbox->run();
	delete sandbox;

	return 0;
}
