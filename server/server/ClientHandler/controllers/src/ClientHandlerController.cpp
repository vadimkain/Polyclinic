#include "ClientHandlerController.hpp"
#include "ClientHandlerModel.hpp"

#include "config.hpp"
#include "EnumStringConvertor.hpp"
#include "HttpHeaders.hpp"
#include "Socket.hpp"
#include "Logger.hpp"

#include <thread>
#include <unordered_map>

namespace server::client_handler::controllers {

ClientHandlerController::ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model,
    std::weak_ptr<context_handler::view::IContextHandlerInterface> context_handler_interface
)   : m_SERVER_STARTER_MODEL{server_model}, m_context_handler_interface{context_handler_interface.lock()}
    // , on_page_address_updated_slot{std::bind<void(std::string)>(&ClientHandlerController::on_page_address_updated, this, std::placeholders::_1)}
    , on_page_address_updated_slot{std::bind(&ClientHandlerController::on_page_address_updated, this, std::placeholders::_1)}
{
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());
}

ClientHandlerController::~ClientHandlerController(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("destructor called on thread #", std::this_thread::get_id());
    for (auto &client : m_client_handler_model_container) {
        client->socket().close();

        if (!client->socket().is_valid()) {
            BLOG_DEBUG("client with fd ", client->socket().m_socket_fd, " is closed");
        } else {
            BLOG_WARNING("Impossible to close the client with fd ", client->socket().m_socket_fd);
        }
    }
}

void ClientHandlerController::start(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    init();

    catch_new_connection();
}

void ClientHandlerController::init() {
    common::connect(&m_context_handler_interface->page_address_updated, &on_page_address_updated_slot);

}

void ClientHandlerController::on_page_address_updated(std::string address) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("Page address = ", address);
}

void ClientHandlerController::catch_new_connection(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    auto server_socket = m_SERVER_STARTER_MODEL->socket();
    while (server_socket.m_socket_fd > 0) {
        BLOG_DEBUG("waiting for connection");

        auto client_socket = server_socket.accept();
        if (client_socket.m_socket_fd < 0) {
            BLOG_ERROR("impossible to connect to client: ", client_socket.latest_error());
            continue;
        }

        handle_connect(client_socket);
    }
}

void ClientHandlerController::read_data(std::weak_ptr<models::IClientHandlerModel> weak_client) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string());

    std::string read_buf;
    auto bytes_read = client->socket().read(read_buf, common::config::BUFFER_SIZE);

    handle_read(client, std::move(read_buf), bytes_read);
}

void ClientHandlerController::send_data(std::weak_ptr<models::IClientHandlerModel> weak_client) {

}

void ClientHandlerController::handle_connect(const common::Socket& client_socket) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = std::make_shared<models::ClientHandlerModel>();

    BLOG_INFO("client: ", client->socket().to_string());

    client->set_socket(std::move(client_socket));
    m_client_handler_model_container.insert(client);

    std::thread(&ClientHandlerController::read_data, this, client).detach();
}

void ClientHandlerController::handle_read(std::weak_ptr<models::IClientHandlerModel> weak_client, std::string&& read_data, std::int32_t bytes_read) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string());
    BLOG_DEBUG("bytes_read = ", bytes_read, "; data = ", read_data);

    common::HttpHeaders http_headers(read_data);

    handle_http_request(client, http_headers);
}

void ClientHandlerController::handle_http_request(std::weak_ptr<models::IClientHandlerModel> weak_client, const common::HttpHeaders &header) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string(), "; http request type = ", common::EnumStringConvertor::init()->to_string(header.method), "; http body = ", header.body);
    m_context_handler_interface->request_to_open_uri(header.uri);
}

}   // !server::client_handler::controllers;