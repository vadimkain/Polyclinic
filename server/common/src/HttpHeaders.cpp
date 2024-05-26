#include "HttpHeaders.hpp"
#include "EnumStringConvertor.hpp"

#include <sstream>
// #include <cinttypes>

namespace server::common {

HttpHeaders::HttpHeaders(const std::string& request) {
    std::istringstream iss(request);
    std::string line;

    // Парсим первую строку для извлечения метода запроса, URI и версии HTTP протокола
    // parse the first line to get request type (method), uri, http version
    std::getline(iss, line);
    std::istringstream first_line_stream(line);
    std::string method_str;
    first_line_stream >> method_str >> uri >> http_version;

    method = EnumStringConvertor::init()->str_to_request_type(method_str);

    // parse least;
    while (std::getline(iss, line) && !line.empty()) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 2); // to skip ": "
            headers.emplace(name, value);
        }
    }

    // Парсим тело сообщения
    std::stringstream body_stream;
    body_stream << iss.rdbuf();
    body = body_stream.str();
}

}   // !server::common;