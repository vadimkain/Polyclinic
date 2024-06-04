#ifndef POLYCLINIC_SERVER_COMMON_CONFIG_HPP
#define POLYCLINIC_SERVER_COMMON_CONFIG_HPP

#include <string>
#include <cinttypes>

namespace server::common::config {

const std::string   LOCALHOST   {"127.0.0.1"};
const std::string   DB_HOST     {LOCALHOST};
const std::string   DB_USERNAME {"mbukhta"};
const std::string   DB_PASSWORD {"1qa@WS3ed"};
const std::string   DB_NAME     {"polyclinic"};
const std::uint16_t DB_PORT {5432};

const std::uint16_t SERVER_PORT {8081};
const std::uint8_t SERVER_LISTEN_SIZE = 10;

const char STR_EOF = '\0';
const char EXTENSTION_SEPARATOR = '.';
const std::uint16_t BUFFER_SIZE = 2048;

const std::string DEFAULT_HTML_FILE = "index.html";

}   // !server::common::config;

#endif  // !POLYCLINIC_SERVER_COMMON_CONFIG_HPP;