#pragma once

#ifndef SPDLOG_H
#include "spdlog/spdlog.h"
#endif

#if CONF_DEBUG
#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#else
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#endif

#if !CONF_RETAIL
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#else
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_CRITICAL(...)
#endif

#define LOG_INFO_ALWAYS(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN_ALWAYS(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR_ALWAYS(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL_ALWAYS(...) SPDLOG_CRITICAL(__VA_ARGS__)