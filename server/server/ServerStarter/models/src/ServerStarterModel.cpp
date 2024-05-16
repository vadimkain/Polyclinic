#include "ServerStarterModel.hpp"

namespace server::serverstarter::models {

common::Socket ServerStarterModel::socket() const {
    std::lock_guard guard(m_socket_mutex);
    
    return m_socket;
}

void ServerStarterModel::set_socket(const common::Socket& val) {
    std::lock_guard guard(m_socket_mutex);
    
    m_socket = val;
}

void ServerStarterModel::set_socket(common::Socket&& val) {
    std::lock_guard guard(m_socket_mutex);
    
    m_socket = std::move(val);
}

}   // !server::serverstarter::models;