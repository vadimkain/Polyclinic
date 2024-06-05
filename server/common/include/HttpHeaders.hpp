#ifndef POLYCLINIC_SERVER_COMMON_HTTP_HEADERS_HPP
#define POLYCLINIC_SERVER_COMMON_HTTP_HEADERS_HPP

#include <string>
#include <unordered_map>

namespace server::common {

class HttpHeaders {
public:
    // Internal classes STARTS

    enum class HttpRequestType {
        NONE,
        GET,
        POST,
        PUT,
        DELETE
    };

    // Internal classes ENDS

public:
    HttpRequestType method; // Тип HTTP запроса (GET, POST, PUT, и т.д.)
    std::string uri; // URI ресурса
    std::string http_version; // Версия HTTP протокола
    std::unordered_multimap<std::string, std::string> headers; // Множество заголовков
    std::string body; // Тело сообщения

    HttpHeaders(void) = default;
    HttpHeaders(const std::string& request);

    std::string extension_to_content_type(const std::string& extension) const;
};

}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_HTTP_HEADERS_HPP;