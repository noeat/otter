#include "log.h"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/null_sink.h"
#include "utilities/common_util.h"
#include "spdlog/logger.h"
#include <map>
#include <algorithm>
#include <iterator>

static spdlog::level::level_enum cast_log_level(const std::string& level)
{
	const static std::map<std::string, spdlog::level::level_enum> mm =
	{
		{"trace", spdlog::level::trace},
		{"debug", spdlog::level::debug},
		{"info", spdlog::level::info},
		{"warn", spdlog::level::warn},
		{"err", spdlog::level::err},
		{"critical", spdlog::level::critical},
		{"off", spdlog::level::off},
	};

	auto itr = mm.find(level);
	return (itr == mm.end() ? spdlog::level::err : itr->second);
}

static spdlog::sink_ptr sink_factory(const std::string& logdir, const std::string& name, const YAML::Node& node)
{
	spdlog::sink_ptr sink = nullptr;
	std::string type = node["type"].as<std::string>();
	if (type == "stdout_color_sink_mt")
	{
		sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	}
	else if (type == "stdout_color_sink_st")
	{
		sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
	}
	else if (type == "stderr_color_sink_mt")
	{
		sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
	}
	else if (type == "stderr_color_sink_st")
	{
		sink = std::make_shared<spdlog::sinks::stderr_color_sink_st>();
	}
	else if (type == "basic_file_sink_mt")
	{
		std::string filename = logdir + "/" + name + ".log";
		sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
	}
	else if (type == "basic_file_sink_st")
	{
		std::string filename = logdir + "/" + name + ".log";
		sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(filename);
	}
	else if (type == "rotating_file_sink_mt")
	{
		std::string filename = logdir + "/" + name + ".log";
		int max_size = node["max_size"].as<int>();
		int max_file = node["max_file"].as<int>();
		sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, max_size, max_file);
	}
	else if (type == "rotating_file_sink_st")
	{
		std::string filename = logdir + "/" + name + ".log";
		int max_size = node["max_size"].as<int>();
		int max_file = node["max_file"].as<int>();
		sink = std::make_shared<spdlog::sinks::rotating_file_sink_st>(filename, max_size, max_file);
	}
	else if (type == "daily_file_sink_mt")
	{
		std::string filename = logdir + "/" + name + ".log";
		int rotation_hour = node["rotation_hour"].as<int>();
		int rotation_minute = node["rotation_minute"].as<int>();
		sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filename, rotation_hour, rotation_minute);
	}
	else if (type == "daily_file_sink_st")
	{
		std::string filename = logdir + "/" + name + ".log";
		int rotation_hour = node["rotation_hour"].as<int>();
		int rotation_minute = node["rotation_minute"].as<int>();
		sink = std::make_shared<spdlog::sinks::daily_file_sink_st>(filename, rotation_hour, rotation_minute);
	}
	else if (type == "null_sink_mt")
	{
		sink = std::make_shared<spdlog::sinks::null_sink_mt>();
	}
	else if (type == "null_sink_st")
	{
		sink = std::make_shared<spdlog::sinks::null_sink_st>();
	}
#ifdef _WIN32
	else if (type == "windebug_sink_mt")
	{
		sink = std::make_shared<spdlog::sinks::windebug_sink_mt>();
	}
	else if (type == "windebug_sink_st")
	{
		sink = std::make_shared<spdlog::sinks::windebug_sink_st>();
	}	
#endif //_WIN32
	else
	{
		std::cerr << "unknow log sink type: " << type << std::endl;
		assert(false);
	}

	std::string level = node["level"].as<std::string>();
	sink->set_level(cast_log_level(level));
	return sink;
}

static void logger_factory(std::map<std::string, spdlog::sink_ptr>& sinks, const YAML::Node& node)
{
	std::string name = node["name"].as<std::string>();
	std::string ssinks = node["sinks"].as<std::string>();
	std::string level = node["level"].as<std::string>();
	bool is_async = node["is_async"].as<bool>();
	std::string pattern = node["pattern"].as<std::string>();
	std::vector<spdlog::sink_ptr> logger_sinks;
	Tokenizer tokenizer(ssinks, '|');
	std::transform(tokenizer.begin(), tokenizer.end(), 
		std::back_inserter(logger_sinks),
		[&sinks, &name](char const* s)
		{
			auto itr = sinks.find(std::string(s));
			if (itr == sinks.end())
			{
				std::cerr << "logger: " << name <<
					" sink: " << s << " cant found." << std::endl;
				assert(false);
			}

			return itr->second;
		});

	std::shared_ptr<spdlog::logger> logger = nullptr;
	if (is_async)
	{
		logger = std::make_shared<spdlog::async_logger>(name,
			logger_sinks.begin(), logger_sinks.end(),
			spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
	}
	else
	{
		logger = std::make_shared<spdlog::logger>(name,
			logger_sinks.begin(), logger_sinks.end());
	}

	logger->set_level(cast_log_level(level));
	logger->set_pattern(pattern);
	spdlog::register_logger(logger);
}

bool init_log_conf(const char* conffile)
{
	if (conffile == nullptr || conffile[0] == 0)
	{
		std::cerr << "init_log_conf: cant find conffile" << std::endl;
		return false;
	}

	try
	{
		YAML::Node config = YAML::LoadFile(conffile);
		std::string logpath = config["logdir"].as<std::string>();
		boost::filesystem::path path(logpath);
		if (boost::filesystem::exists(path))
		{
			boost::filesystem::create_directory(logpath);
		}

		if (config["thread_pool_queue_size"])
		{
			int thread_pool_queue_size = config["thread_pool_queue_size"].as<int>();
			int thread_pool_thread_count = config["thread_pool_thread_count"].as<int>();
			spdlog::init_thread_pool(thread_pool_queue_size, thread_pool_thread_count);
		}
		
		if (!config["sinks"])
		{
			std::cerr << "cant find sinks" << std::endl;
			return false;
		}

		std::map<std::string, spdlog::sink_ptr> sinks;
		for (auto itr = config["sinks"].begin(); itr != config["sinks"].end(); ++itr)
		{
			YAML::Node& node = *itr;
			if (node["name"])
			{
				std::string name = node["name"].as<std::string>();
				sinks[name] = sink_factory(logpath, name, node);
			}
		}

		if (!config["loggers"])
		{
			std::cerr << "cant find loggers" << std::endl;
			return false;
		}
	}
	catch (std::exception& e)
	{
		std::cout << "init_log_conf exception: "
			<< e.what() << std::endl;
		return false;
	}

	return true;	
}

void shutdown_log()
{
	//spdlog::flush_every(std::chrono::seconds(3));
	spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->info("log service shutdown."); });
	spdlog::shutdown();
}

static void do_log(const std::string& logger, spdlog::level::level_enum level, const char* format, va_list ap)
{
	auto log = spdlog::get(logger);
	if (log && log->should_log(level))
	{
		char buff[2048];
		vsprintf(buff, format, ap);
		va_end(ap);
		log->log(level, buff);
	}	
}

void log_trace(const std::string& logger, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	do_log(logger, spdlog::level::trace, format, ap);
}

void log_debug(const std::string& logger, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	do_log(logger, spdlog::level::debug, format, ap);
}

void log_info(const std::string& logger, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	do_log(logger, spdlog::level::info, format, ap);
}

void log_warn(const std::string& logger, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	do_log(logger, spdlog::level::warn, format, ap);
}

void log_error(const std::string& logger, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	do_log(logger, spdlog::level::err, format, ap);
}

void log_critical(const std::string& logger, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	do_log(logger, spdlog::level::critical, format, ap);
}

bool should_log(const std::string& logger, int level)
{
	auto log = spdlog::get(logger);
	return (log && log->should_log((spdlog::level::level_enum)level));
}

std::shared_ptr<spdlog::logger> get_logger(const std::string& logger)
{
	auto log = spdlog::get(logger);
	return log;
}