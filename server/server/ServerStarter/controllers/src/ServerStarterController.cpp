#include "ServerStarterController.hpp"

#include "Logger.hpp"

#include <thread>

namespace server::serverstarter::controllers {

ServerStarterController::ServerStarterController(std::weak_ptr<models::IServerStarterModel> model)
    // : m_server_starter_model{model.lock()}
{
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);

    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());
}

void ServerStarterController::start(void) {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);

    BLOG_INFO("called");
}

}   // !server::serverstarter::controllers;