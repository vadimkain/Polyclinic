#include "Logger.hpp"

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <sstream>

namespace bukhtalog {

/* Description:
 *  The implementation of this method in hpp file because the link error is happened
 * due to using macros. Please check Logger.hpp file to check the implementation;
 */
// inline void __init_logger__(const std::string &name, bool show_console);

Logger* Logger::get_instance() {
    static Logger logger;
    return &logger;
}

void Logger::init() {
    // Do not use the break in switch...case because we go from up to down to set all logging lvls;
    switch (m_logging_lvl) {
        case LoggingEnum::CONSOLE_LOG: {
            static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
            plog::init<static_cast<int8_t>(LoggingEnum::CONSOLE_LOG)>(plog::verbose, &consoleAppender);
        }
        case LoggingEnum::FILE_LOG: {
            plog::init<static_cast<int8_t>(LoggingEnum::CONSOLE_LOG)>(plog::verbose, m_log_file_name.c_str());
        }
    }
}

void Logger::set_log_file_name(const std::string &val) {
    m_log_file_name = val;
}

void Logger::set_logging_lvl(LoggingEnum val) noexcept {
    m_logging_lvl = val;
}   

}   // !bukhtalog;