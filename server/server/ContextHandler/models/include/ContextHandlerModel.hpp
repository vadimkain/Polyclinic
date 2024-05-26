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

private:
    mutable std::mutex m_current_uri_mutex;
    std::string m_current_uri;
};

}   // !server::context_handler::models;

#endif  // !POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_MODEL_CONTEXTHANDLERMODEL_HPP;