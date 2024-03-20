#ifndef BUKHTALOG_LOGGER_HPP
#define BUKHTALOG_LOGGER_HPP

#include <string>
#include <sstream>
#include <cinttypes>
#include <initializer_list>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/ColorConsoleAppender.h>

class Logger {
public:
    enum class LoggingEnum : int8_t {
        FILE_LOG = 0,
        CONSOLE_LOG = 1
    };

public:
    static Logger* get_instance();
    void init();
    void set_log_file_name(const std::string &val);
    void set_logging_lvl(LoggingEnum val) noexcept;

    template<typename... Args>
    inline void log(plog::Severity log_lvl, const Args& ...args);

private:
    Logger() = default;

    template<typename T, typename... Args>
    void args_to_string(std::stringstream& log_stream, const T& val, const Args& ...args);
    template<typename T>
    void args_to_string(std::stringstream& log_stream, const T& val);

private:
    LoggingEnum m_logging_lvl;
    std::string m_log_file_name;
};

template<typename... Args>
void Logger::log(plog::Severity log_lvl, const Args& ...args) {
    std::stringstream log_stream;

    args_to_string(log_stream, args...);
    
    // for loop is not possible due to some compilation problems;
    switch (m_logging_lvl) {
        case LoggingEnum::CONSOLE_LOG: {
            PLOG_(static_cast<int32_t>(LoggingEnum::CONSOLE_LOG), log_lvl) << log_stream.str();
        }
        case LoggingEnum::FILE_LOG: {
            PLOG_(static_cast<int32_t>(LoggingEnum::FILE_LOG), log_lvl) << log_stream.str();
        }
    }
}

// The implementation of this method in hpp file because the link error is happened due to using macros;
template<typename T, typename... Args>
void Logger::args_to_string(std::stringstream& log_stream, const T& val, const Args& ...args) {
    log_stream << val;
    args_to_string(log_stream, args...);
}

// The implementation of this method in hpp file because the link error is happened due to using macros;
template<typename T>
void Logger::args_to_string(std::stringstream& log_stream, const T& val) {
    log_stream << val;
}

/* Description:
 *  This macro should be used in the main.cpp file to init the logger and use it without
 * any errors. The implementation of this method in hpp file because the link error is happened
 * due to using macros;
 */
inline void __init_logger__(const std::string& name, bool show_console) {
    auto __logger__ = Logger::get_instance();
    __logger__->set_log_file_name(name);
    if (show_console) {
        __logger__->set_logging_lvl(Logger::LoggingEnum::CONSOLE_LOG);
    }
    __logger__->init();
}

inline std::string construct_method_name(const std::string& class_name, const std::string& method_name) {
    std::stringstream full_name_stream;
    full_name_stream << "[" << class_name << "][" << method_name << "] ";
    
    return std::move(full_name_stream.str());
}

/*
 * Logging macros starts;
 */

#ifndef BLOG_INIT
#define BLOG_INIT(name, show_console)   \
    __init_logger__(name, show_console);
#endif // !LOG_INIT;

#ifndef BDECLARE_TAG_SCOPE
#define BDECLARE_TAG_SCOPE(CLASS_NAME, METHOD_NAME)  \
    std::string __function_name__ = std::move(construct_method_name(CLASS_NAME, METHOD_NAME));
#endif  // !BDECLARE_TAG_SCOPE

#ifndef BLOG
#define BLOG(LOG_LVL, ...)  \
    Logger::get_instance()->log(LOG_LVL, __function_name__, __VA_ARGS__);
#endif  // !BLOG

/*
 * Logging Lvl macros starts;
 */

#ifndef BLOG_VERBOSE
#define BLOG_VERBOSE(...)   \
    BLOG(plog::verbose, __VA_ARGS__);
#endif  // !BLOG_VERBOSE;

#ifndef BLOG_DEBUG
#define BLOG_DEBUG(...) \
    BLOG(plog::debug, __VA_ARGS__);
#endif  // !BLOG_DEBUG;

#ifndef BLOG_INFO
#define BLOG_INFO(...)  \
    BLOG(plog::info, __VA_ARGS__);
#endif  // !BLOG_INFO;

#ifndef BLOG_WARNING
#define BLOG_WARNING(...)   \
    BLOG(plog::warning, __VA_ARGS__);
#endif  // !BLOG_WARNING;

#ifndef BLOG_ERROR
#define BLOG_ERROR(...) \
    BLOG(plog::error, __VA_ARGS__);
#endif  // !BLOG_ERROR;

#ifndef BLOG_FATAL
#define BLOG_FATAL(...) \
    BLOG(plog::fatal, __VA_ARGS__);
#endif  // !BLOG_FATAL;

/*
 * Logging Lvl macros ends;
 */

/*
 * Logging macros ends;
 */

#endif // !BUKHTALOG_LOGGER_HPP
