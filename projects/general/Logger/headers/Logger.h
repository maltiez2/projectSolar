#pragma once

#include <memory>
#include <string>

#include "spdlog/spdlog.h"

#ifdef DEBUG_LOGGING
	#define LOG_DEBUG(message) projectSolar::Logger::debug(message)
#else
	#define LOG_DEBUG(message)
#endif

#define LOG_INTT_FILE(logfile) projectSolar::Logger::init({ 5 * 1024 * 1024 * 8, 3, logfile, true, false })
#define LOG_INTT_CONSOLE(logfile) projectSolar::Logger::init({ 5 * 1024 * 1024 * 8, 3, logfile, true, true })
#define LOG_INFO(message) projectSolar::Logger::info(message)
#define LOG_WARN(message) projectSolar::Logger::warn(message)
#define LOG_ERROR(message) projectSolar::Logger::error(message)
#define LOG_CRITICAL(message) projectSolar::Logger::critical(message)
#define LOG_ONLY_CRITICAL projectSolar::Logger::filterCritical()
#define LOG_SET_PATTERN(message) projectSolar::Logger::setPattern(message)

namespace projectSolar
{
	class Logger
	{
	public:
		static constexpr char const* defaultLoggerName = "defaultLogger";
		static constexpr char const* defaultPattern = "[%H:%M:%S.%e] [thread: %5t] [%l] %v";
		static constexpr char const* defaultColoredPattern = "%^[%H:%M:%S.%e] [thread: %5t] [%l] %v%$";
		static constexpr int flushEvery_seconds = 1;

		struct LoggerParameters
		{
			size_t maxLogSize_bits;
			size_t maxFilesNumber;
			std::string fileName;
			bool rotateOnOpen;
			bool toConsole;
		};

		static void init(const LoggerParameters& params);
		static void debug(std::string_view);
		static void info(std::string_view);
		static void warn(std::string_view);
		static void error(std::string_view);
		static void critical(std::string_view);

		static void filterCritical();
		static void setPattern(std::string_view);

	private:
		std::shared_ptr<spdlog::logger> m_spdLogger;

		static Logger& get();

		Logger() = default;
		~Logger();
		void setParams(const LoggerParameters& params);
	};
}
