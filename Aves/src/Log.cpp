#include "Log.hpp"

namespace Aves {

	std::shared_ptr<spdlog::logger> Log::s_Logger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_Logger = spdlog::stdout_color_mt("AVES");
		s_Logger->set_level(spdlog::level::trace);
	}

} //namespace Aves