#ifndef POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_MODEL_CONTEXTHANDLERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_MODEL_CONTEXTHANDLERMODEL_HPP

#include "IContextHandlerModel.hpp"

#include <string>
#include <mutex>

namespace server::context_handler::models {

class ContextHandlerModel : public IContextHandlerModel {
public:
    std::string current_uri(void) const;
    void set_current_uri(const std::string& val);
    void set_current_uri(std::string&& val);

    std::string current_page_file(void) const override;
    void set_current_page_file(const std::string& val) override;
    void set_current_page_file(std::string&& val) override;

private:
    mutable std::mutex m_current_uri_mutex;
    std::string m_current_uri;

    mutable std::mutex m_current_page_file_mutex;
    std::string m_current_page_file;
};

}   // !server::context_handler::models;

#endif  // !POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_MODEL_CONTEXTHANDLERMODEL_HPP;