#ifndef POLYCLINIC_SERVER_COMMON_SOCKET_HPP
#define POLYCLINIC_SERVER_COMMON_SOCKET_HPP

#include <cinttypes>

#include <netinet/in.h>

namespace server::common {
    
struct Socket {
public:
    bool operator== (const Socket& other);
    bool operator!= (const Socket& other);
    bool operator< (const Socket& other);
    bool operator> (const Socket& other);

public:
    std::int32_t m_socket_fd = 0;
    sockaddr_in m_sock_address;
};

}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_SOCKET_HPP;