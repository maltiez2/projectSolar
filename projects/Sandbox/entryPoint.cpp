#include "pch.h"

#include "Core.h"

#include "Profiler.h"


using namespace projectSolar;

int main()
{
	LOG_INTT_CONSOLE("logs/log_sandbox.txt");
	LOG_DEBUG("[sandbox] Sandbox started");
	PROFILE_BEGIN("Sandbox");

	Application app;
	app.run();

	PROFILE_END();
	LOG_DEBUG("[sandbox] Sandbox finished");
	return 0;
}