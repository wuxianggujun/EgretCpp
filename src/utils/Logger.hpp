/**
 * @file Logger.hpp
 * @brief Logger类 - 日志系统
 * 
 * 翻译自：egret-core-5.4.1/src/egret/utils/Logger.ts
 * Logger是一个静态日志类，提供分级日志功能，与TypeScript版本完全兼容。
 */

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <fstream>

namespace egret {

/**
 * @brief Logger类 - 日志系统
 * 
 * Logger是一个静态日志类，提供与TypeScript版本完全兼容的分级日志功能。
 * 支持ALL、DEBUG、INFO、WARN、ERROR、OFF六个日志级别。
 * 
 * @note 对应TypeScript的egret.Logger类
 * @version Egret 2.4
 * @platform Web,Native
 */
class Logger {
public:
    /**
     * @brief 日志级别枚举
     * 
     * 对应TypeScript中的字符串常量定义
     */
    enum class Level {
        ALL = 0,    ///< 显示所有日志 - 对应 "all"
        DEBUG = 1,  ///< 调试及以上级别 - 对应 "debug"  
        INFO = 2,   ///< 信息及以上级别 - 对应 "info"
        WARN = 3,   ///< 警告及以上级别 - 对应 "warn"
        ERROR = 4,  ///< 仅错误日志 - 对应 "error"
        OFF = 5     ///< 关闭日志 - 对应 "off"
    };

    // ========== 静态常量 - 兼容TypeScript API ==========
    
    /**
     * @brief 显示所有日志级别 - 对应 public static ALL:string = "all"
     */
    static const std::string ALL;
    
    /**
     * @brief 调试级别 - 对应 public static DEBUG:string = "debug"
     */
    static const std::string DEBUG;
    
    /**
     * @brief 信息级别 - 对应 public static INFO:string = "info"
     */
    static const std::string INFO;
    
    /**
     * @brief 警告级别 - 对应 public static WARN:string = "warn"
     */
    static const std::string WARN;
    
    /**
     * @brief 错误级别 - 对应 public static ERROR:string = "error"
     */
    static const std::string ERROR;
    
    /**
     * @brief 关闭日志 - 对应 public static OFF:string = "off"
     */
    static const std::string OFF;

    // ========== 日志级别控制 ==========
    
    /**
     * @brief 设置日志级别 (字符串版本) - 兼容TypeScript API
     * @param level 日志级别字符串 ("all", "debug", "info", "warn", "error", "off")
     */
    static void setLogLevel(const std::string& level);
    
    /**
     * @brief 设置日志级别 (枚举版本) - C++优化API
     * @param level 日志级别枚举
     */
    static void setLogLevel(Level level);
    
    /**
     * @brief 获取当前日志级别
     * @return 当前日志级别枚举
     */
    static Level getLogLevel();
    
    /**
     * @brief 获取当前日志级别字符串
     * @return 当前日志级别字符串
     */
    static std::string getLogLevelString();

    /**
     * 设置日志文件输出
     * @param filepath 日志文件路径
     * @param append 是否追加写入
     */
    static void setLogFile(const std::string& filepath, bool append = true);

    /**
     * 启用/禁用控制台输出
     */
    static void setConsoleEnabled(bool enabled);

    // ========== 日志输出方法 ==========
    
    /**
     * @brief 调试日志输出
     * @param message 日志消息
     * @param file 文件名（通常使用__FILE__宏）
     * @param line 行号（通常使用__LINE__宏）
     * @param func 函数名（通常使用__FUNCTION__宏）
     */
    static void debug(const std::string& message, const char* file = nullptr, int line = 0, const char* func = nullptr);
    
    /**
     * @brief 信息日志输出
     * @param message 日志消息
     * @param file 文件名（通常使用__FILE__宏）
     * @param line 行号（通常使用__LINE__宏）
     * @param func 函数名（通常使用__FUNCTION__宏）
     */
    static void info(const std::string& message, const char* file = nullptr, int line = 0, const char* func = nullptr);
    
    /**
     * @brief 警告日志输出
     * @param message 日志消息
     * @param file 文件名（通常使用__FILE__宏）
     * @param line 行号（通常使用__LINE__宏）
     * @param func 函数名（通常使用__FUNCTION__宏）
     */
    static void warn(const std::string& message, const char* file = nullptr, int line = 0, const char* func = nullptr);
    
    /**
     * @brief 错误日志输出
     * @param message 日志消息
     * @param file 文件名（通常使用__FILE__宏）
     * @param line 行号（通常使用__LINE__宏）
     * @param func 函数名（通常使用__FUNCTION__宏）
     */
    static void error(const std::string& message, const char* file = nullptr, int line = 0, const char* func = nullptr);
    
    // ========== 格式化日志方法 (C++扩展) ==========
    
    /**
     * @brief 格式化调试日志输出
     * @param file 文件名
     * @param line 行号
     * @param func 函数名
     * @param format 格式化字符串
     * @param args 格式化参数
     */
    template<typename... Args>
    static void debugf(const char* file, int line, const char* func, const std::string& format, Args&&... args) {
        if (shouldLog(Level::DEBUG)) {
            logFormatted(Level::DEBUG, file, line, func, format, std::forward<Args>(args)...);
        }
    }
    
    /**
     * @brief 格式化信息日志输出
     * @param file 文件名
     * @param line 行号
     * @param func 函数名
     * @param format 格式化字符串
     * @param args 格式化参数
     */
    template<typename... Args>
    static void infof(const char* file, int line, const char* func, const std::string& format, Args&&... args) {
        if (shouldLog(Level::INFO)) {
            logFormatted(Level::INFO, file, line, func, format, std::forward<Args>(args)...);
        }
    }
    
    /**
     * @brief 格式化警告日志输出
     * @param file 文件名
     * @param line 行号
     * @param func 函数名
     * @param format 格式化字符串
     * @param args 格式化参数
     */
    template<typename... Args>
    static void warnf(const char* file, int line, const char* func, const std::string& format, Args&&... args) {
        if (shouldLog(Level::WARN)) {
            logFormatted(Level::WARN, file, line, func, format, std::forward<Args>(args)...);
        }
    }
    
    /**
     * @brief 格式化错误日志输出
     * @param file 文件名
     * @param line 行号
     * @param func 函数名
     * @param format 格式化字符串
     * @param args 格式化参数
     */
    template<typename... Args>
    static void errorf(const char* file, int line, const char* func, const std::string& format, Args&&... args) {
        if (shouldLog(Level::ERROR)) {
            logFormatted(Level::ERROR, file, line, func, format, std::forward<Args>(args)...);
        }
    }

    // ========== 实用工具方法 ==========
    
    /**
     * @brief 判断指定级别是否应该输出日志
     * @param level 要检查的日志级别
     * @return 是否应该输出
     */
    static bool shouldLog(Level level);
    
    /**
     * @brief 将日志级别枚举转换为字符串
     * @param level 日志级别枚举
     * @return 日志级别字符串
     */
    static std::string levelToString(Level level);
    
    /**
     * @brief 将字符串转换为日志级别枚举
     * @param levelStr 日志级别字符串
     * @return 日志级别枚举
     */
    static Level stringToLevel(const std::string& levelStr);

    // ========== 便利宏定义 ==========

    /**
     * @brief 提取文件名（不含路径）
     * @param filepath 完整文件路径
     * @return 文件名
     */
    static const char* extractFileName(const char* filepath);

private:
    /**
     * @brief 当前日志级别
     */
    static Level s_currentLevel;
    
    /**
     * @brief 是否已初始化
     */
    static bool s_initialized;
    static bool s_consoleEnabled;
    static bool s_logToFile;
    static std::ofstream s_logFile;
    
    /**
     * @brief 初始化日志系统
     */
    static void initialize();
    
    /**
     * @brief 执行实际的日志输出
     * @param level 日志级别
     * @param message 日志消息
     * @param file 文件名
     * @param line 行号
     * @param func 函数名
     */
    static void doLog(Level level, const std::string& message, const char* file = nullptr, int line = 0, const char* func = nullptr);

    /**
     * @brief 简化函数名：去掉命名空间/类限定，只保留短函数名
     * @param func 原始函数名（可能包含命名空间/类）
     * @return 去限定后的短函数名（例如 "drawDisplayObject"）
     */
    static std::string simplifyFunction(const char* func);
    
    /**
     * @brief 格式化日志输出的内部实现
     * @param level 日志级别
     * @param file 文件名
     * @param line 行号
     * @param func 函数名
     * @param format 格式化字符串
     * @param args 格式化参数
     */
    template<typename... Args>
    static void logFormatted(Level level, const char* file, int line, const char* func, const std::string& format, Args&&... args) {
        // 简单的字符串格式化实现
        std::ostringstream oss;
        formatHelper(oss, format, std::forward<Args>(args)...);
        doLog(level, oss.str(), file, line, func);
    }
    
    /**
     * @brief 格式化辅助方法 - 递归终止
     * @param oss 输出流
     * @param format 剩余格式化字符串
     */
    static void formatHelper(std::ostringstream& oss, const std::string& format) {
        oss << format;
    }
    
    /**
     * @brief 格式化辅助方法 - 递归实现
     * @param oss 输出流
     * @param format 格式化字符串
     * @param value 当前参数值
     * @param args 剩余参数
     */
    template<typename T, typename... Args>
    static void formatHelper(std::ostringstream& oss, const std::string& format, T&& value, Args&&... args) {
        auto pos = format.find("{}");
        if (pos != std::string::npos) {
            oss << format.substr(0, pos) << value;
            formatHelper(oss, format.substr(pos + 2), std::forward<Args>(args)...);
        } else {
            oss << format << " " << value;
            formatHelper(oss, "", std::forward<Args>(args)...);
        }
    }
};

// ========== 便利宏定义 - 自动包含文件名、行号、函数名 ==========

/**
 * @brief 调试日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_DEBUG(message) \
    egret::Logger::debug(message, egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__)

/**
 * @brief 信息日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_INFO(message) \
    egret::Logger::info(message, egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__)

/**
 * @brief 警告日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_WARN(message) \
    egret::Logger::warn(message, egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__)

/**
 * @brief 错误日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_ERROR(message) \
    egret::Logger::error(message, egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__)

/**
 * @brief 格式化调试日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_DEBUGF(format, ...) \
    egret::Logger::debugf(egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__, format, __VA_ARGS__)

/**
 * @brief 格式化信息日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_INFOF(format, ...) \
    egret::Logger::infof(egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__, format, __VA_ARGS__)

/**
 * @brief 格式化警告日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_WARNF(format, ...) \
    egret::Logger::warnf(egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__, format, __VA_ARGS__)

/**
 * @brief 格式化错误日志宏 - 自动包含文件名、行号、函数名
 */
#define EGRET_ERRORF(format, ...) \
    egret::Logger::errorf(egret::Logger::extractFileName(__FILE__), __LINE__, __FUNCTION__, format, __VA_ARGS__)

} // namespace egret
