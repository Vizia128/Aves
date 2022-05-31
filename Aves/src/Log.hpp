#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Aves {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; };

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};

} //namespace Aves


// Log Macros
#define AVES_TRACE(...) ::Aves::Log::GetLogger()->trace(__VA_ARGS__)
#define AVES_INFO(...) ::Aves::Log::GetLogger()->info(__VA_ARGS__)
#define AVES_WARN(...) ::Aves::Log::GetLogger()->warn(__VA_ARGS__)
#define AVES_ERROR(...) ::Aves::Log::GetLogger()->error(__VA_ARGS__)
#define AVES_FATAL(...) ::Aves::Log::GetLogger()->fatal(__VA_ARGS__)