#include "Socket.hpp"

namespace server::common {

bool Socket::operator== (const Socket& other) {
    return this->m_socket_fd == other.m_socket_fd;
}

bool Socket::operator!= (const Socket& other) {
    return this->m_socket_fd != other.m_socket_fd;
}

bool Socket::operator< (const Socket& other) {
    return this->m_socket_fd < other.m_socket_fd;
}

bool Socket::operator> (const Socket& other) {
    return this->m_socket_fd > other.m_socket_fd;
}

}   // !server::common;