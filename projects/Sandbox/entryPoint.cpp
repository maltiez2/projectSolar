#include "pch.h"

#include "Core.h"


using namespace projectSolar;

int main()
{
	LOG_INTT_CONSOLE("logs/log_sandbox.txt");
	LOG_DEBUG("[sandbox] Sandbox started");

	Application app;
	app.run();

	LOG_DEBUG("[sandbox] Sandbox finished");
	return 0;
}