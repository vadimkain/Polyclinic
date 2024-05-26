#include "EnumStringConvertor.hpp"

namespace server::common {

EnumStringConvertor::EnumStringConvertor(void) 
    : m_http_request_types {
        {HttpHeaders::HttpRequestType::NONE, "NONE"},
        {HttpHeaders::HttpRequestType::GET, "GET"},
        {HttpHeaders::HttpRequestType::POST, "POST"},
        {HttpHeaders::HttpRequestType::PUT, "PUT"},
        {HttpHeaders::HttpRequestType::DELETE, "DELETE"}
    }
{

}

EnumStringConvertor* EnumStringConvertor::init() {
    static EnumStringConvertor instance;

    return &instance;
}

std::string EnumStringConvertor::to_string(HttpHeaders::HttpRequestType val) {
    return m_http_request_types[val];
}

HttpHeaders::HttpRequestType EnumStringConvertor::str_to_request_type(std::string val) {
    for (const auto& elem_it : m_http_request_types) {
        if (elem_it.second == val) {
            return elem_it.first;
        }
    }

    return HttpHeaders::HttpRequestType::NONE;
}

}   // !server::common;