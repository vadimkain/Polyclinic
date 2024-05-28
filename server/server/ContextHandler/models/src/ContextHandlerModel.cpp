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

std::string ContextHandlerModel::current_page_file(void) const {
    std::lock_guard guard(m_current_page_file_mutex);
    return m_current_page_file;
}

void ContextHandlerModel::set_current_page_file(const std::string& val) {
    std::lock_guard guard(m_current_page_file_mutex);
    m_current_page_file = val;
}

void ContextHandlerModel::set_current_page_file(std::string&& val) {
    std::lock_guard guard(m_current_page_file_mutex);
    m_current_page_file = std::move(val);
}

}   // !server::context_handler::models;