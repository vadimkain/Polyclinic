#include "ContextHandlerModel.hpp"

namespace server::context_handler::models {

std::string ContextHandlerModel::current_uri(void) const {
    std::lock_guard guard(m_current_uri_mutex);
    return m_current_uri;
}

void ContextHandlerModel::set_current_uri(const std::string& val) {
    std::lock_guard guard(m_current_uri_mutex);
    m_current_uri = val;
}

void ContextHandlerModel::set_current_uri(std::string&& val) {
    std::lock_guard guard(m_current_uri_mutex);
    m_current_uri = std::move(val);
}

}   // !server::context_handler::models;