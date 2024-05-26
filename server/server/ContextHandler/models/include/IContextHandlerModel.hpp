#ifndef POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_MODEL_ICONTEXTHANDLERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_MODEL_ICONTEXTHANDLERMODEL_HPP

#include "IContextHandlerModel.hpp"

#include <string>

namespace server::context_handler::models {

class IContextHandlerModel {
public:
    virtual std::string current_uri(void) const = 0;
    virtual void set_current_uri(const std::string& val) = 0;
    virtual void set_current_uri(std::string&& val) = 0;
};

}   // !server::context_handler::models;

#endif  // !POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_MODEL_ICONTEXTHANDLERMODEL_HPP;