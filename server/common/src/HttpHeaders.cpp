#include "HttpHeaders.hpp"
#include "EnumStringConvertor.hpp"

#include <sstream>
// #include <cinttypes>

namespace server::common {

HttpHeaders::HttpHeaders(const std::string& request) {
    std::istringstream iss(request);
    std::string line;

    // parse the first line to get request type (method), uri, http version
    std::getline(iss, line);
    std::istringstream first_line_stream(line);
    std::string method_str;
    first_line_stream >> method_str >> uri >> http_version;

    method = EnumStringConvertor::init()->str_to_request_type(method_str);

    // parse least;
    while (std::getline(iss, line) && !line.empty()) {
        size_t pos = line.find(':');

        if (pos == std::string::npos) {
            break;
        }
        std::string name = line.substr(0, pos);
        std::string value = line.substr(pos + 2); // to skip ": "
        headers.emplace(name, value);
    }

    // Парсим тело сообщения
    std::stringstream body_stream;
    body_stream << iss.rdbuf();
    body = body_stream.str();
}

std::string HttpHeaders::extension_to_content_type(const std::string& extension) const {
    std::stringstream ret_ss;

    if (extension == "html") {
        ret_ss << "Content-Type: text/html\r\n";
    } else if (extension == "css") {
        ret_ss << "Content-Type: text/css\r\n";
    } else if (extension == "jpg" || extension == "jpeg") {
        ret_ss << "Content-Type: image/jpeg\r\n";
    } else if (extension == "png") {
        ret_ss << "Content-Type: image/png\r\n";
    } else if (extension == "svg") {
        ret_ss << "Content-Type: image/svg+xml\r\n";
    } else if (extension.find("wof") != std::string::npos) {
        ret_ss << "Content-Type: font/woff\r\n";
    } else if (extension == "ico") {
        ret_ss << "Content-Type: image/vnd.microsoft.icon\r\n";
    } else if (extension == "json") {
        ret_ss << "Content-Type: application/json\r\n";
    }
    else {
        ret_ss << "Content-Type: application/octet-stream\r\n";
    }

    return ret_ss.str();
}

}   // !server::common;