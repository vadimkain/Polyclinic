#ifndef POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP
#define POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP

#include "IServerStarterModel.hpp"
#include "IClientHandlerInterface.hpp"
#include "ClientHandlerController.hpp"

#include <memory>

namespace server::serverstarter::controllers {

class ServerStarterController {
public:
    ServerStarterController(std::weak_ptr<models::IServerStarterModel> model, 
        std::weak_ptr<context_handler::view::IContextHandlerInterface> context_handler_interface,
        std::weak_ptr<client_handler::view::IClientHandlerInterface> client_handler_interface
    );
    ~ServerStarterController(void);

    void start(void);
    void close(void);

private:
    void run_server();

private:
    std::shared_ptr<context_handler::view::IContextHandlerInterface> m_context_handler_interface;
    std::shared_ptr<client_handler::view::IClientHandlerInterface> m_client_handler_interface;

    std::shared_ptr<models::IServerStarterModel> m_server_starter_model;

    // controllers;
    std::shared_ptr<client_handler::controllers::ClientHandlerController> m_client_handler_controller;
};

}   // !server::serverstarter::controllers;

#endif  // !POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP;