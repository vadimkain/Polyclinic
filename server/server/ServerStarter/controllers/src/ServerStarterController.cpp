#include "ServerStarterController.hpp"

#include "Logger.hpp"

#include <thread>

// #include <unistd.h>

namespace server::serverstarter::controllers {

ServerStarterController::ServerStarterController(std::weak_ptr<models::IServerStarterModel> model)
    : m_server_starter_model{model.lock()}
{
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);

    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());
}

ServerStarterController::~ServerStarterController(void) {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);
    BLOG_INFO("destructor called on thread #", std::this_thread::get_id());
    
    close();
}

void ServerStarterController::start(void) {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);

    while (true) {};

    BLOG_INFO("called");
}

void ServerStarterController::close(void) {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);
    BLOG_INFO("called");
    std::int32_t err = 0;

    if (err = ::close(m_server_starter_model->socket_fd())) {
        BLOG_ERROR("impossible to close the server fd: ", m_server_starter_model->socket_fd(), ". Error #", err);
        err = 0;
    }
    m_server_starter_model->set_socket_fd(0);
}

}   // !server::serverstarter::controllers;