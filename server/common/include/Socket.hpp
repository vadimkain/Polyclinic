#ifndef POLYCLINIC_SERVER_COMMON_SOCKET_HPP
#define POLYCLINIC_SERVER_COMMON_SOCKET_HPP

#include "File.hpp"

#include <string>
#include <cinttypes>

#include <netinet/in.h>

namespace server::common {
    
class Socket {
public:
    Socket();
    Socket(const Socket& other) = default;
    Socket(Socket&& other);
    ~Socket();

    constexpr Socket& operator= (const Socket& other) = default;
    Socket& operator= (Socket&& other);
    bool operator== (const Socket& other) const;
    bool operator!= (const Socket& other) const;
    bool operator< (const Socket& other) const;
    bool operator> (const Socket& other) const;

    std::string to_string() const;

    std::int32_t init();
    std::int32_t deinit();

    void set_port(uint16_t port);
    std::int32_t bind();
    std::int32_t listen(std::int32_t count);
    Socket accept();
    std::int32_t send(const std::string& data);
    std::int32_t sendfile(const File& file);
    std::int32_t read(std::string &ret_buf, std::int32_t max_buf_size);
    std::int32_t close();

    std::string latest_error() const;
    bool is_valid() const;
    std::string ip_address_as_string() const;

private:
    std::int32_t getsockname(Socket &sock) const;

private:
    std::int32_t m_socket_fd = 0;
    sockaddr_in m_sock_address;
};

}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_SOCKET_HPP;