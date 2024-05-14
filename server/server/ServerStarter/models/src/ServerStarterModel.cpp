#include "ServerStarterModel.hpp"

namespace server::serverstarter::models {

ServerStarterModel::ServerStarterModel(void)
    : m_socket_fd{0}
{

}

std::int32_t ServerStarterModel::socket_fd() const noexcept {
    return m_socket_fd.load(std::memory_order::memory_order_acquire);
}

void ServerStarterModel::set_socket_fd(std::int32_t val) noexcept {
    m_socket_fd.exchange(val, std::memory_order::memory_order_release);
}

}   // !server::serverstarter::models;