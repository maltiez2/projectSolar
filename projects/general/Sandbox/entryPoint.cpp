#define PROFILING

#pragma warning (disable: 4703)

#include "Core.h"

#include "Logger.h"
#include "Profiler.h"


int main()
{
	LOG_INTT_CONSOLE("logs/log_sandbox.txt");
	LOG_DEBUG("[sandbox] Sandbox started");
	PROFILE_BEGIN("sandbox");

	auto* app = new projectSolar::Application();
	app->run();
	delete(app);

	PROFILE_END();
	LOG_DEBUG("[sandbox] Sandbox finished");
	return 0;
}
