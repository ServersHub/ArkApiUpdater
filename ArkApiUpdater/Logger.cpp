#include "Logger.h"

Log::Log()
{
	Logger_ = spdlog::stdout_color_st("ArkApi AutoUpdate");
}

Log& Log::Get()
{
	static Log instance;
	return instance;
}

std::shared_ptr<spdlog::logger> Log::GetLog()
{
	return Get().Logger_;
}