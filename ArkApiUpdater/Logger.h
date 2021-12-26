#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Log
{
public:
	Log();

	static Log& Get();
	static std::shared_ptr<spdlog::logger> GetLog();
private:
	std::shared_ptr<spdlog::logger> Logger_;
};