#ifndef __common_log_h__
#define __common_log_h__
#include "spdlog/spdlog.h"
#include <memory>
bool init_log_conf(const char* conffile);
void shutdown_log();

void log_trace(const std::string& logger, const char* format, ...);
void log_debug(const std::string& logger, const char* format, ...);
void log_info(const std::string& logger, const char* format, ...);
void log_warn(const std::string& logger, const char* format, ...);
void log_error(const std::string& logger, const char* format, ...);
void log_critical(const std::string& logger, const char* format, ...);
bool should_log(const std::string& logger, int level);
std::shared_ptr<spdlog::logger> get_logger(const std::string& logger);

#define LOG_LOGGER_TRACE SPDLOG_LOGGER_TRACE
#define LOG_TRACE(loggername, ...)  SPDLOG_LOGGER_TRACE(spdlog::get(loggername), __VA_ARGS__)
#define LOG_DEFAULT_TRACE SPDLOG_TRACE

#define LOG_LOGGER_DEBUG SPDLOG_LOGGER_DEBUG
#define LOG_DEBUG(loggername, ...) SPDLOG_LOGGER_DEBUG(spdlog::get(loggername), __VA_ARGS__)
#define LOG_DEFAULT_DEBUG SPDLOG_DEBUG

#define LOG_LOGGER_INFO SPDLOG_LOGGER_INFO
#define LOG_INFO(loggername, ...) SPDLOG_LOGGER_INFO(spdlog::get(loggername), __VA_ARGS__)
#define LOG_DEFAULT_INFO SPDLOG_INFO

#define LOG_LOGGER_WARN SPDLOG_LOGGER_WARN
#define LOG_WARN(loggername, ...) SPDLOG_LOGGER_WARN(spdlog::get(loggername), __VA_ARGS__)
#define LOG_DEFAULT_WARN SPDLOG_WARN

#define LOG_LOGGER_ERROR SPDLOG_LOGGER_ERROR
#define LOG_ERROR(loggername, ...) SPDLOG_LOGGER_ERROR(spdlog::get(loggername), __VA_ARGS__)
#define LOG_DEFAULT_ERROR SPDLOG_ERROR

#define LOG_LOGGER_CRITICAL SPDLOG_LOGGER_CRITICAL
#define LOG_CRITICAL(loggername, ...) SPDLOG_LOGGER_CRITICAL(spdlog::get(loggername), __VA_ARGS__)
#define LOG_DEFAULT_CRITICAL SPDLOG_CRITICAL
#endif //__common_log_h__