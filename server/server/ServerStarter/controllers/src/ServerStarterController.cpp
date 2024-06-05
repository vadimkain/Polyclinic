#include "ServerStarterController.hpp"

#include "Logger.hpp"
#include "config.hpp"

#include <thread>
#include <algorithm>

// #include <unistd.h>

namespace server::serverstarter::controllers {

ServerStarterController::ServerStarterController(std::weak_ptr<models::IServerStarterModel> model, 
    std::weak_ptr<context_handler::view::IContextHandlerInterface> context_handler_interface,
    std::weak_ptr<client_handler::view::IClientHandlerInterface> client_handler_interface
) : m_server_starter_model{model.lock()}, m_context_handler_interface{context_handler_interface.lock()},
    m_client_handler_interface{client_handler_interface.lock()}
{
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);
    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());

    m_client_handler_controller = std::make_shared<client_handler::controllers::ClientHandlerController>(m_server_starter_model,
        m_client_handler_interface, m_context_handler_interface
    );
}

ServerStarterController::~ServerStarterController(void) {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);
    BLOG_INFO("destructor called on thread #", std::this_thread::get_id());
    
    close();
}

void ServerStarterController::start(void) {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);
    BLOG_INFO("called");

    run_server();

    std::thread(&client_handler::controllers::ClientHandlerController::start, m_client_handler_controller.get()).join();
}

void ServerStarterController::close(void) {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);
    BLOG_INFO("called");
    std::int32_t err = 0;

    auto server_socket = m_server_starter_model->socket();

    if (!server_socket.is_valid()) {
        BLOG_WARNING("Server Socket is not valid");
    }
    if (server_socket.close()) {
        BLOG_ERROR("impossible to close the server ", server_socket.to_string(),  ". ", server_socket.latest_error());
        err = 0;
    } else {
        m_server_starter_model->set_socket(server_socket);
    }

}

void ServerStarterController::run_server() {
    BDECLARE_TAG_SCOPE("ServerStarterController", __FUNCTION__);
    BLOG_INFO("called");

    auto server_socket = m_server_starter_model->socket();
    server_socket.set_port(common::config::SERVER_PORT);

    if (server_socket.init()) {
        BLOG_FATAL("Server socket create error. ", server_socket.latest_error());
        abort();
    }
    if (server_socket.bind()) {
        BLOG_ERROR("impossible to bind", server_socket.latest_error());
    }
    if (server_socket.listen(common::config::SERVER_LISTEN_SIZE)) {
        BLOG_ERROR("impossible to listen", server_socket.latest_error());
    }

    m_server_starter_model->set_socket(std::move(server_socket));
}

}   // !server::serverstarter::controllers;