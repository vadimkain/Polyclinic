#ifndef POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_CONTROLLERS_CONTEXTHANDLERCONTROLLER_HPP
#define POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_CONTROLLERS_CONTEXTHANDLERCONTROLLER_HPP

#include "IContextHandlerModel.hpp"

#include <memory>
#include <string>

namespace server::context_handler::controllers {

class ContextHandlerController {
public:
    ContextHandlerController(std::weak_ptr<models::IContextHandlerModel> model);

    void open_default_page_by_uri(const std::string& uri);
private:
    std::shared_ptr<models::IContextHandlerModel> m_context_handler_model;
};

}   // !server::context_handler::controllers;

#endif  // !POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_CONTROLLERS_CONTEXTHANDLERCONTROLLER_HPP;