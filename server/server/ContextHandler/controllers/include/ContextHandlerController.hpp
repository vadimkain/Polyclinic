#ifndef POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_CONTROLLERS_CONTEXTHANDLERCONTROLLER_HPP
#define POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_CONTROLLERS_CONTEXTHANDLERCONTROLLER_HPP

#include "IContextHandlerModel.hpp"
#include "IContextHandlerInterface.hpp"

#include <memory>
#include <string>

namespace server::context_handler::controllers {

class ContextHandlerController {
public:
    ContextHandlerController(std::weak_ptr<models::IContextHandlerModel> model,
        std::weak_ptr<view::IContextHandlerInterface> view
    );

    void init();

    void open_default_page_by_uri(const std::string& uri);

/*
 * Slots starts;
 */
public:
    common::Slot<std::string, common::Socket> on_open_uri_slot;
    void on_open_uri(std::string address, common::Socket socket);

/*
 * Slots ends;
 */

private:
    std::shared_ptr<models::IContextHandlerModel> m_context_handler_model;
    std::shared_ptr<view::IContextHandlerInterface> m_context_handler_interface;
};

}   // !server::context_handler::controllers;

#endif  // !POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_CONTROLLERS_CONTEXTHANDLERCONTROLLER_HPP;