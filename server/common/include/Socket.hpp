#ifndef POLYCLINIC_SERVER_COMMON_SOCKET_HPP
#define POLYCLINIC_SERVER_COMMON_SOCKET_HPP

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
    constexpr Socket& operator= (Socket&& other);
    constexpr bool operator== (const Socket& other);
    constexpr bool operator!= (const Socket& other);
    constexpr bool operator< (const Socket& other);
    constexpr bool operator> (const Socket& other);

    std::int32_t init();

    std::int32_t deinit();

    void set_port(uint16_t port);
    std::int32_t bind();
    std::int32_t listen(std::int32_t count);
    Socket accept();
    // std::int32_t connect();
    // std::int32_t write();
    // std::int32_t read();
    std::int32_t close();

    std::string latest_error() const;
    bool is_valid() const;

private:
    std::int32_t getsockname(Socket &sock) const;

public:
    std::int32_t m_socket_fd = 0;
    sockaddr_in m_sock_address;
};

}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_SOCKET_HPP;