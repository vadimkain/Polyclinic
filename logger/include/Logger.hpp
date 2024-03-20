#ifndef BUKHTALOG_LOGGER_HPP
#define BUKHTALOG_LOGGER_HPP

#include <string>
#include <sstream>
#include <cinttypes>
#include <initializer_list>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/ColorConsoleAppender.h>

namespace bukhtalog {

class Logger {
public:
    enum class LoggingEnum : int8_t {
        FILE_LOG = 0,
        CONSOLE_LOG = 1
    };

public:
    /* Description:
     *  This class was constructed with Singleton pattern.
     * This method should return the pointer to the single instance
     * of this class;
     * 
     * RETURN:
     *  Pointer to the single instance of this class;
     */
    static Logger* get_instance();

    /* Description:
     *  Create the logger objects as specified earlier with set methods;
     */
    void init();

    /* Description:
     *  Set log file name
     *
     * ARGS:
     *  val - const std::string& - file name where we should write logs;
     */
    void set_log_file_name(const std::string &val);

    /* Description:
     *  Set logging lvl. It means how many type of logs should be created.
     * For example should it be logging into file and into console.
     *
     * ARGS:
     *  val - LoggingEnum - logging identifier;
     */
    void set_logging_lvl(LoggingEnum val) noexcept;

    /* Description:
     *  Create log with specific log_lvl;
     *
     * ARGS:
     *  log_lvl - plog::Severity: in which lvl should be written the log;
     *  ...args - const Args&: infinity buffer of log information what should be written;
     */
    template<typename... Args>
    inline void log(plog::Severity log_lvl, const Args& ...args);

private:
    Logger() = default;

    /* Description:
     *  write infinity args to the log_stream to get all neccessary information as
     * string to log it;
     * 
     * ARGS:
     *  log_stream - std::stringstream&: converted args buffer to string;
     *  val - const T&: current arg that should be written to the buffer;
     *  ...args - const Args&: infinity buffer of log information what should be written;
     */
    template<typename T, typename... Args>
    void args_to_string(std::stringstream& log_stream, const T& val, const Args& ...args);
    
    /* Description:
     *  write infinity args to the log_stream to get all neccessary information as
     * string to log it;
     * 
     * ARGS:
     *  log_stream - std::stringstream&: converted args buffer to string;
     *  val - const T&: current arg that should be written to the buffer;
     */
    template<typename T>
    void args_to_string(std::stringstream& log_stream, const T& val);

private:
    LoggingEnum m_logging_lvl;
    std::string m_log_file_name;
};

/* Description:
 *  This macro should be used in the main.cpp file to init the logger and use it without
 * any errors.
 * 
 * ARGS:
 * name - const std::string&: Log file name;
 * show_console - bool: should logs be shown in the console.
 *              true - show logs in console;
 *              false - don't output logs into console;
 */
inline void __init_logger__(const std::string& name, bool show_console);

/* Description:
 *  Contruct the string with class name and method name for log output inside brackets:
 * The return value have the next form:
 *      [class_name][method name] ;
 * 
 * ARGS:
 *  class_name - const std::string&: class name that should be shown in the logs;
 *  method_name - const std::string&: method name that should be shown in the logs;
 *
 * RETURN:
 *  Formated string with class name and method in view as <<[class_name][method name] >>;
 */
inline std::string construct_method_name(const std::string& class_name, const std::string& method_name);

/*
 * Logger methods for log macroses implementation STARTS;
 * 
 * !!! WARNING !!!
 *  The implementation of these method in hpp file because the link error is happened 
 * due to using macros (Undefined reference);
 */

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

template<typename T, typename... Args>
void Logger::args_to_string(std::stringstream& log_stream, const T& val, const Args& ...args) {
    log_stream << val;
    args_to_string(log_stream, args...);
}

template<typename T>
void Logger::args_to_string(std::stringstream& log_stream, const T& val) {
    log_stream << val;
}

/*
 * Logger methods for log macroses implementation ENDS;
 * 
 * !!! WARNING !!!
 *  The implementation of these method in hpp file because the link error is happened 
 * due to using macros (Undefined reference);
 */

/*
 * Local methods for log macroses implementation STARTS;
 * 
 * !!! WARNING !!!
 *  The implementation of these method in hpp file because the link error is happened 
 * due to using macros (Undefined reference);
 */

void __init_logger__(const std::string& name, bool show_console) {
    auto __logger__ = Logger::get_instance();
    __logger__->set_log_file_name(name);
    if (show_console) {
        __logger__->set_logging_lvl(Logger::LoggingEnum::CONSOLE_LOG);
    }
    __logger__->init();
}

std::string construct_method_name(const std::string& class_name, const std::string& method_name) {
    std::stringstream full_name_stream;
    full_name_stream << "[" << class_name << "][" << method_name << "] ";
    
    return std::move(full_name_stream.str());
}

/*
 * Local methods for log macroses implementation ENDS;
 * 
 * !!! WARNING !!!
 *  The implementation of these method in hpp file because the link error is happened 
 * due to using macros (Undefined reference);
 */

}   // !bukhtalog;

/*
 * Logging macros STARTS;
 */

#ifndef BLOG_INIT
#define BLOG_INIT(name, show_console)   \
    bukhtalog::__init_logger__(name, show_console);
#endif // !LOG_INIT;

#ifndef BDECLARE_TAG_SCOPE
#define BDECLARE_TAG_SCOPE(CLASS_NAME, METHOD_NAME)  \
    std::string __function_name__ = std::move(bukhtalog::construct_method_name(CLASS_NAME, METHOD_NAME));
#endif  // !BDECLARE_TAG_SCOPE

#ifndef BLOG
#define BLOG(LOG_LVL, ...)  \
    bukhtalog::Logger::get_instance()->log(LOG_LVL, __function_name__, __VA_ARGS__);
#endif  // !BLOG

/*
 * Logging Lvl macros STARTS;
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
 * Logging Lvl macros ENDS;
 */

/*
 * Logging macros ENDS;
 */

#endif // !BUKHTALOG_LOGGER_HPP
