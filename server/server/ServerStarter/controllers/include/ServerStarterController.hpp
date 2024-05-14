#ifndef POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP
#define POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP

#include "IServerStarterModel.hpp"

#include <memory>

namespace server::serverstarter::controllers {

class ServerStarterController {
public:
    ServerStarterController(std::weak_ptr<models::IServerStarterModel> model);

    void start(void);

private:
    std::shared_ptr<models::IServerStarterModel> m_server_starter_model;
};

}   // !server::serverstarter::controllers;

#endif  // !POLYCLINIC_SERVER_SERVER_SERVERSTARTER_CONTROLLERS_SERVERSTARTERCONTROLLER_HPP;