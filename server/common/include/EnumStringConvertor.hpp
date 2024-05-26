#ifndef POLYCLINIC_SERVER_COMMON_ENUM_STRING_CONVERTOR_HPP
#define POLYCLINIC_SERVER_COMMON_ENUM_STRING_CONVERTOR_HPP

#include "HttpHeaders.hpp"

#include <string>
#include <unordered_map>

namespace server::common {

class EnumStringConvertor {
private:
    EnumStringConvertor(void);

public:
    static EnumStringConvertor* init();

    std::string to_string(HttpHeaders::HttpRequestType val);

    HttpHeaders::HttpRequestType str_to_request_type(std::string val);

private:
    std::unordered_map<HttpHeaders::HttpRequestType, std::string> m_http_request_types;
};

}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_ENUM_STRING_CONVERTOR_HPP;