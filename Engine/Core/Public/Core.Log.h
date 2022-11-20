#pragma once

#ifndef SPDLOG_H
#include "spdlog/spdlog.h"
#endif

// Spamming output while locally debugging.
// Should not be triggered for anyone but the individual debugging.
#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)

// General output available only in debug builds
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)

// General output available in all builds except final
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
// Error output avilable in all builds (including final)
#define LOG_INFO_ALWAYS(...) SPDLOG_INFO(__VA_ARGS__)

// Warning output available in all builds except final
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
// Error output avilable in all builds (including final)
#define LOG_WARN_ALWAYS(...) SPDLOG_WARN(__VA_ARGS__)

// Error output available in all builds except final
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
// Error output avilable in all builds (including final)
#define LOG_ERROR_ALWAYS(...) SPDLOG_ERROR(__VA_ARGS__)

// Critical output available in all builds except final
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
// Critical output avilable in all builds (including final)
#define LOG_CRITICAL_ALWAYS(...) SPDLOG_CRITICAL(__VA_ARGS__)