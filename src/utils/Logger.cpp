/**
 * @file Logger.cpp
 * @brief Logger类实现 - 日志系统
 * 
 * 翻译自：egret-core-5.4.1/src/egret/utils/Logger.ts
 * Logger是一个静态日志类，提供分级日志功能，与TypeScript版本完全兼容。
 */

#include "utils/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace egret {

// ========== 静态成员变量定义 ==========

Logger::Level Logger::s_currentLevel = Level::INFO;
bool Logger::s_initialized = false;

// ========== TypeScript兼容的字符串常量定义 ==========

const std::string Logger::ALL = "all";
const std::string Logger::DEBUG = "debug";
const std::string Logger::INFO = "info";
const std::string Logger::WARN = "warn";
const std::string Logger::ERROR = "error";
const std::string Logger::OFF = "off";

// ========== 日志级别控制实现 ==========

void Logger::setLogLevel(const std::string& level) {
    if (!s_initialized) {
        initialize();
    }
    s_currentLevel = stringToLevel(level);
}

void Logger::setLogLevel(Level level) {
    if (!s_initialized) {
        initialize();
    }
    s_currentLevel = level;
}

Logger::Level Logger::getLogLevel() {
    if (!s_initialized) {
        initialize();
    }
    return s_currentLevel;
}

std::string Logger::getLogLevelString() {
    return levelToString(getLogLevel());
}

// ========== 日志输出方法实现 ==========

void Logger::debug(const std::string& message, const char* file, int line, const char* func) {
    if (shouldLog(Level::DEBUG)) {
        doLog(Level::DEBUG, message, file, line, func);
    }
}

void Logger::info(const std::string& message, const char* file, int line, const char* func) {
    if (shouldLog(Level::INFO)) {
        doLog(Level::INFO, message, file, line, func);
    }
}

void Logger::warn(const std::string& message, const char* file, int line, const char* func) {
    if (shouldLog(Level::WARN)) {
        doLog(Level::WARN, message, file, line, func);
    }
}

void Logger::error(const std::string& message, const char* file, int line, const char* func) {
    if (shouldLog(Level::ERROR)) {
        doLog(Level::ERROR, message, file, line, func);
    }
}

// ========== 实用工具方法实现 ==========

bool Logger::shouldLog(Level level) {
    if (!s_initialized) {
        initialize();
    }
    return level >= s_currentLevel && s_currentLevel != Level::OFF;
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::ALL: return ALL;
        case Level::DEBUG: return DEBUG;
        case Level::INFO: return INFO;
        case Level::WARN: return WARN;
        case Level::ERROR: return ERROR;
        case Level::OFF: return OFF;
        default: return INFO;
    }
}

Logger::Level Logger::stringToLevel(const std::string& levelStr) {
    std::string lowerStr = levelStr;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    
    if (lowerStr == "all") return Level::ALL;
    if (lowerStr == "debug") return Level::DEBUG;
    if (lowerStr == "info") return Level::INFO;
    if (lowerStr == "warn") return Level::WARN;
    if (lowerStr == "error") return Level::ERROR;
    if (lowerStr == "off") return Level::OFF;
    
    return Level::INFO; // 默认级别
}

const char* Logger::extractFileName(const char* filepath) {
    if (!filepath) {
        return "unknown";
    }
    
    // 从路径中提取文件名（支持Windows和Unix路径分隔符）
    const char* lastSlash = strrchr(filepath, '/');
    const char* lastBackslash = strrchr(filepath, '\\');
    const char* fileName = filepath;
    
    if (lastSlash && lastBackslash) {
        fileName = (lastSlash > lastBackslash) ? lastSlash + 1 : lastBackslash + 1;
    } else if (lastSlash) {
        fileName = lastSlash + 1;
    } else if (lastBackslash) {
        fileName = lastBackslash + 1;
    }
    
    return fileName;
}

// ========== 私有方法实现 ==========

void Logger::initialize() {
    if (s_initialized) {
        return;
    }
    
    s_initialized = true;
    
    // 在Debug模式下默认开启DEBUG级别
    #ifdef _DEBUG
        s_currentLevel = Level::DEBUG;
    #else
        s_currentLevel = Level::INFO;
    #endif
    
    // 输出初始化信息
    std::cout << "[EGRET] Logger initialized with level: " 
              << levelToString(s_currentLevel) << std::endl;
}

void Logger::doLog(Level level, const std::string& message, const char* file, int line, const char* func) {
    if (!shouldLog(level)) {
        return;
    }
    
    // 获取当前时间戳
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // 格式化时间
    std::ostringstream timeStr;
    timeStr << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    timeStr << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // 获取级别字符串和颜色
    std::string levelStr = levelToString(level);
    std::string colorCode;
    std::string resetCode = "\033[0m";
    
    // 根据级别设置颜色（支持终端颜色的环境）
    switch (level) {
        case Level::DEBUG:
            colorCode = "\033[36m"; // 青色
            break;
        case Level::INFO:
            colorCode = "\033[32m"; // 绿色
            break;
        case Level::WARN:
            colorCode = "\033[33m"; // 黄色
            break;
        case Level::ERROR:
            colorCode = "\033[31m"; // 红色
            break;
        default:
            colorCode = "";
            resetCode = "";
            break;
    }
    
    // 选择输出流
    std::ostream& output = (level >= Level::WARN) ? std::cerr : std::cout;
    
    // 构建位置信息字符串
    std::ostringstream locationStr;
    if (file && line > 0) {
        locationStr << "[" << file << ":" << line;
        if (func) {
            locationStr << " in " << func << "()";
        }
        locationStr << "] ";
    }
    
    // 输出格式：[时间] [级别] [文件:行号 in 函数()] 消息
    output << colorCode 
           << "[" << timeStr.str() << "] "
           << "[" << std::setw(5) << levelStr << "] "
           << locationStr.str()
           << message 
           << resetCode << std::endl;
}

} // namespace egret