#include "Logger.h"

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"

namespace projectSolar
{
	void Logger::setParams(const LoggerParameters& params)
	{
		if (params.toConsole)
		{
			m_spdLogger = spdlog::stdout_color_mt<spdlog::async_factory>(defaultLoggerName);

			std::vector<spdlog::sink_ptr> sinks;
			sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
			sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(params.fileName, params.maxLogSize_bits, params.maxFilesNumber, params.rotateOnOpen));
			sinks[0]->set_pattern(defaultColoredPattern);
			sinks[1]->set_pattern(defaultPattern);
			m_spdLogger = std::make_shared<spdlog::logger>(defaultLoggerName, begin(sinks), end(sinks));
		}
		else
		{
			m_spdLogger = spdlog::rotating_logger_mt<spdlog::async_factory>(defaultLoggerName, params.fileName, params.maxLogSize_bits, params.maxFilesNumber, params.rotateOnOpen);
			m_spdLogger->set_pattern(defaultPattern);
		}
		m_spdLogger->set_level(spdlog::level::level_enum::debug);
		spdlog::flush_every(std::chrono::seconds(flushEvery_seconds));
		m_spdLogger->info("log started");
	}
	Logger::~Logger()
	{

	}

	Logger& Logger::get()
	{
		static Logger instance;
		return instance;
	}

	void Logger::init(const LoggerParameters& params)
	{
		static bool initialized;
		if (initialized)
		{
			return;
		}
		get().setParams(params);
		initialized = true;
		return;
	}
	void Logger::debug(std::string_view input)
	{
		get().m_spdLogger->debug(input);
	}
	void Logger::info(std::string_view input)
	{
		get().m_spdLogger->info(input);
	}
	void Logger::warn(std::string_view input)
	{
		get().m_spdLogger->warn(input);
	}
	void Logger::error(std::string_view input)
	{
		get().m_spdLogger->error(input);
	}
	void Logger::critical(std::string_view input)
	{
		get().m_spdLogger->critical(input);
	}
	void Logger::filterCritical()
	{
		get().m_spdLogger->set_level(spdlog::level::level_enum::critical);
	}
	void Logger::setPattern(std::string_view input)
	{
		get().m_spdLogger->set_pattern(input.data());
	}
}