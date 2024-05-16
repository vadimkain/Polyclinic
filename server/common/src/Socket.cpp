#include "Socket.hpp"
#include "config.hpp"
#include "Logger.hpp"

#include <algorithm>

#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace server::common {

Socket::Socket()
    : m_socket_fd{0}
{
    m_sock_address.sin_addr.s_addr = INADDR_ANY;
    m_sock_address.sin_family = AF_INET;
    m_sock_address.sin_port = ::htons(0);
    std::fill(m_sock_address.sin_zero, m_sock_address.sin_zero + sizeof(m_sock_address.sin_zero), config::STR_EOF);
}

Socket::Socket(Socket&& other) {
    operator=(std::move(other));
}

Socket::~Socket() {
    // temporary commented due to closing fd after destroy
    // bug description;
    // when we copied socket in some area and the life cycle is ending
    // this object is destroyed and the other stay in life.
    // Due to the fd becomed invalid.
    // deinit();
}

constexpr Socket& Socket::operator= (Socket&& other) {
    this->m_socket_fd = other.m_socket_fd;
    other.m_socket_fd = 0;  // null the socket to escape the closing;
    this->m_sock_address = other.m_sock_address;

    return *this;
}

constexpr bool Socket::operator== (const Socket& other) {
    return this->m_socket_fd == other.m_socket_fd;
}

constexpr bool Socket::operator!= (const Socket& other) {
    return this->m_socket_fd != other.m_socket_fd;
}

constexpr bool Socket::operator< (const Socket& other) {
    return this->m_socket_fd < other.m_socket_fd;
}

constexpr bool Socket::operator> (const Socket& other) {
    return this->m_socket_fd > other.m_socket_fd;
}

std::int32_t Socket::init() {
    BDECLARE_TAG_SCOPE("Socket", __FUNCTION__);
    // сделать этот момент более гибким под разные протоколы
    m_socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);

    return m_socket_fd <= 0 ? -1 : 0;
}

std::int32_t Socket::deinit() {
    return close();
}

void Socket::set_port(uint16_t port) {
    m_sock_address.sin_port = ::htons(port);
}

int Socket::bind() {
    return ::bind(m_socket_fd, reinterpret_cast<sockaddr*>(&m_sock_address), sizeof(m_sock_address));
}

int Socket::listen(std::int32_t count) {
    return ::listen(m_socket_fd, count);
}

Socket Socket::accept() {
    socklen_t address_len = sizeof(m_sock_address);
    Socket accepted_socket;
    accepted_socket.m_socket_fd = ::accept(m_socket_fd, reinterpret_cast<sockaddr*>(&m_sock_address), &address_len);
    getsockname(accepted_socket);
    return accepted_socket;
}

// int Socket::connect() {

// }

// int Socket::write() {

// }

// int Socket::read() {

// }

std::int32_t Socket::close() {
    std::int32_t ret = 0;
    if (m_socket_fd > 0) {
        ret = ::close(m_socket_fd);
    }

    if (!ret) {
        m_socket_fd = 0;
    }
    
    return ret;
}

std::int32_t Socket::getsockname(Socket &sock) const {
    socklen_t sock_address_len = sizeof(sock.m_sock_address);
    return ::getsockname(sock.m_socket_fd, reinterpret_cast<sockaddr*>(&sock.m_sock_address), &sock_address_len);
}

std::string Socket::latest_error() const {
    return std::strerror(errno);
}

bool Socket::is_valid() const {
    return ::fcntl(m_socket_fd, F_GETFL) != -1 || errno != EBADF;
}

}   // !server::common;