#include "ClientHandlerModel.hpp"

namespace server::client_handler::models {

bool ClientHandlerModel::operator== (const ClientHandlerModel& other) {
    return this->m_socket_fd == other.m_socket_fd;
}

bool ClientHandlerModel::operator!= (const ClientHandlerModel& other) {
    return this->m_socket_fd != other.m_socket_fd;
}

bool ClientHandlerModel::operator< (const ClientHandlerModel& other) {
    return this->m_socket_fd < other.m_socket_fd;
}

bool ClientHandlerModel::operator> (const ClientHandlerModel& other) {
    return this->m_socket_fd > other.m_socket_fd;
}

std::int32_t ClientHandlerModel::socket_fd(void) const noexcept {
    return m_socket_fd.load(std::memory_order::memory_order_acquire);
}

void ClientHandlerModel::set_socket_fd(std::int32_t val) noexcept {
    m_socket_fd.exchange(val, std::memory_order::memory_order_release);
}

}   // !server::client_handler::models;