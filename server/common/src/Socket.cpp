#include "Socket.hpp"
#include "config.hpp"
#include "Logger.hpp"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <memory>

#include <arpa/inet.h>
#include <cerrno>
#include <unistd.h>
#include <sys/sendfile.h>

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

Socket& Socket::operator= (Socket&& other) {
    this->m_socket_fd = other.m_socket_fd;
    other.m_socket_fd = 0;  // null the socket to escape the closing;
    this->m_sock_address = other.m_sock_address;

    return *this;
}

bool Socket::operator== (const Socket& other) const {
    return this->m_socket_fd == other.m_socket_fd;
}

bool Socket::operator!= (const Socket& other) const {
    return this->m_socket_fd != other.m_socket_fd;
}

bool Socket::operator< (const Socket& other) const {
    return this->m_socket_fd < other.m_socket_fd;
}

bool Socket::operator> (const Socket& other) const {
    return this->m_socket_fd > other.m_socket_fd;
}

std::string Socket::to_string() const {
    std::stringstream ret;
    ret << "ip = " << ip_address_as_string() << "; port = " << m_sock_address.sin_port
        << "; fd = " << m_socket_fd;

    return ret.str();
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

std::int32_t Socket::send(const std::string& data) {
    return ::send(m_socket_fd, data.c_str(), data.size(), 0);
}

std::int32_t Socket::sendfile(const File& file) {
    off_t offset{0};

    while (offset < file.size()) {
        if(::sendfile(m_socket_fd, file.file_fd(), &offset, file.size() - offset) < 0) {
            break;
        }
    }

    return offset;
}

std::int32_t Socket::read(std::string &ret_buf, std::int32_t max_buf_size) {
    auto read_buf = std::make_unique<char[]>(max_buf_size);
    std::memset(read_buf.get(), 0, max_buf_size);
    std::int32_t bytes_read = ::read(m_socket_fd, read_buf.get(), max_buf_size);

    if (bytes_read > 0) {
        ret_buf.assign(read_buf.get(), bytes_read);
    }

    return bytes_read;
}


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

std::string Socket::ip_address_as_string() const {
    return ::inet_ntoa(m_sock_address.sin_addr);
}

}   // !server::common;