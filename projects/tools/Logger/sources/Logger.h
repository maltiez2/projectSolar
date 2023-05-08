#pragma once

#include <memory>
#include <string>

#include "spdlog/spdlog.h"

#define LOG_INTT_FILE(logfile) projectSolar::Logger::init({ 5 * 1024 * 1024 * 8, 3, logfile, true, false })
#define LOG_INTT_CONSOLE(logfile) projectSolar::Logger::init({ 5 * 1024 * 1024 * 8, 3, logfile, true, true })
#define LOG_ONLY_CRITICAL projectSolar::Logger::filterCritical()
#define LOG_SET_PATTERN(message) projectSolar::Logger::setPattern(message)

#define LOG_INFO(...)     projectSolar::Logger::logMulti<projectSolar::Logger::Severity::info>(__VA_ARGS__)
#define LOG_WARN(...)     projectSolar::Logger::logMulti<projectSolar::Logger::Severity::warn>(__VA_ARGS__)
#define LOG_ERROR(...)    projectSolar::Logger::logMulti<projectSolar::Logger::Severity::error>(__VA_ARGS__)
#define LOG_CRITICAL(...) projectSolar::Logger::logMulti<projectSolar::Logger::Severity::critical>(__VA_ARGS__)

#ifdef DEBUG_LOGGING
	#define LOG_DEBUG(...) projectSolar::Logger::logMulti<projectSolar::Logger::Severity::debug>(__VA_ARGS__)
	#define LOG_ASSERT(x, ...) {if (!(x)) {LOG_ERROR("[assert] Failed assertion at 'line ", __LINE__, "' in '", __FILE__, "': ", __VA_ARGS__); __debugbreak();}}
#else
	#define LOG_DEBUG(...)
	#define LOG_ASSERT(x, ...)
#endif

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

		enum class Severity
		{
			debug,
			info,
			warn,
			error,
			critical
		};

		static void init(const LoggerParameters& params);
		static void filterCritical();
		static void setPattern(std::string_view);

		template<typename T>
		static void logImpl(std::ostringstream& oss, T input)
		{
			oss << input;
		}
		template<typename T, typename ... Args>
		static void logImpl(std::ostringstream& oss, T input, const Args& ... args)
		{
			logImpl(oss, input);
			logImpl(oss, args...);
		}
		template<Severity severity, typename ... Args>
		static void logMulti(const Args& ... args)
		{
			std::ostringstream oss;
			logImpl(oss, args...);
			log<severity>(oss.str());
		}

		template<Severity severity>
		static void log(std::string_view)
		{
			static_assert(true, "Specialize it for a specific type");
		}
		template<>
		static void log<Severity::debug>(std::string_view input)
		{
			get().m_spdLogger->debug(input);
		}
		template<>
		static void log<Severity::info>(std::string_view input)
		{
			get().m_spdLogger->info(input);
		}
		template<>
		static void log<Severity::warn>(std::string_view input)
		{
			get().m_spdLogger->warn(input);
		}
		template<>
		static void log<Severity::error>(std::string_view input)
		{
			get().m_spdLogger->error(input);
		}
		template<>
		static void log<Severity::critical>(std::string_view input)
		{
			get().m_spdLogger->critical(input);
		}

	private:
		std::shared_ptr<spdlog::logger> m_spdLogger;

		static Logger& get();

		Logger() = default;
		~Logger();
		void setParams(const LoggerParameters& params);
	};
}
