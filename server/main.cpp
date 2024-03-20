#include "config.hpp"
#include "DBQuery.hpp"

#include "Logger.hpp"

#include <cinttypes>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace db = server::db;

std::string currentDateTimeAsString() {
    // Get the current time using std::chrono
    auto now = std::chrono::system_clock::now();
    
    // Convert the current time to a time_t object
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    
    // Convert time_t to a struct tm (broken down time)
    std::tm tm_struct = *std::localtime(&now_c);
    
    // Format the date-time as a string
    std::stringstream ss;
    ss << std::put_time(&tm_struct, "%Y.%m.%d_%H.%M.%S");
    
    return ss.str();
}

std::string generate_log_file_name(const std::string &first_name_part) {
    std::stringstream file_name_stream;
    file_name_stream << first_name_part << "_" << currentDateTimeAsString() << ".log";

    return std::move(file_name_stream.str());
}

int main(int argc, char **argv) {
    BLOG_INIT(std::move(generate_log_file_name("server_logs.txt")), true);
    BDECLARE_TAG_SCOPE("", __FUNCTION__);
    
    db::DBQuery dbquery;
    dbquery.output_all_users();

    return 0;
}