#ifndef POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP
#define POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP

#include "ClientHandlerController.hpp"

#include "IServerStarterModel.hpp"

#include <memory>

namespace server::serverstarter::controllers {

class ServerStarterController {
public:
    ServerStarterController(std::weak_ptr<models::IServerStarterModel> model);
    ~ServerStarterController(void);

    void start(void);
    void close(void);

private:
    std::shared_ptr<models::IServerStarterModel> m_server_starter_model;

    // controllers;
    std::shared_ptr<client_handler::controllers::ClientHandlerController> m_client_handler_controller;
};

}   // !server::serverstarter::controllers;

#endif  // !POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP;