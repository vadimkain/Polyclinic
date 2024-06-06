#include "config.hpp"
#include "DBQuery.hpp"
#include "Logger.hpp"

#include "ServerStarterController.hpp"
#include "ContextHandlerController.hpp"
#include "ServerStarterModel.hpp"
#include "ContextHandlerModel.hpp"
#include "ContextHandlerInterface.hpp"
#include "ClientHandlerInterface.hpp"

#include <cinttypes>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <memory>
#include <queue>

#include <sys/stat.h>   // mkdir;

namespace db = server::db;

std::string current_date_time_as_string() {
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
    file_name_stream << first_name_part << "_" << current_date_time_as_string() << ".log";

    return std::move(file_name_stream.str());
}

int main(int argc, char **argv) {
    const std::string LOGS_FOLDER_DIR_NAME = "logs";
    mkdir(LOGS_FOLDER_DIR_NAME.c_str(), 0755);
    BLOG_INIT(std::move(generate_log_file_name(LOGS_FOLDER_DIR_NAME + "/server_logs.txt")), true);
    BDECLARE_TAG_SCOPE("", __FUNCTION__);

    std::queue<std::thread> thread_pull;

    auto context_handler_interface = std::make_shared<server::context_handler::view::ContextHandlerInterface>();
    auto context_handler_model = std::make_shared<server::context_handler::models::ContextHandlerModel>();
    auto context_handler_controller = std::make_shared<server::context_handler::controllers::ContextHandlerController>(context_handler_model, context_handler_interface);

    auto server_starter_model = std::make_shared<server::serverstarter::models::ServerStarterModel>();
    auto client_handler_interface = std::make_shared<server::client_handler::view::ClientHandlerInterface>();
    server::serverstarter::controllers::ServerStarterController server_starter_controller(server_starter_model, 
        context_handler_interface, client_handler_interface
    );

    thread_pull.push(std::thread(&server::serverstarter::controllers::ServerStarterController::start, &server_starter_controller));

    // dbquery.output_all_users();

    while (!thread_pull.empty()) {
        if (thread_pull.front().joinable()) {
            thread_pull.front().join();
        }

        thread_pull.pop();
    }

    return 0;
}