#include "ClientHandlerModel.hpp"

namespace server::client_handler::models {

common::Socket ClientHandlerModel::socket() const {
    std::lock_guard guard(m_socket_mutex);
    
    return m_socket;
}

void ClientHandlerModel::set_socket(const common::Socket& val) {
    std::lock_guard guard(m_socket_mutex);
    
    m_socket = val;
}

void ClientHandlerModel::set_socket(common::Socket&& val) {
    std::lock_guard guard(m_socket_mutex);
    
    m_socket = std::move(val);
}

}   // !server::client_handler::models;