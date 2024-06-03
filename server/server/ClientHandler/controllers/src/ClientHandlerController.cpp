#include "ClientHandlerController.hpp"
#include "ClientHandlerModel.hpp"

#include "config.hpp"
#include "EnumStringConvertor.hpp"
#include "HttpHeaders.hpp"
#include "Socket.hpp"
#include "Logger.hpp"

#include <thread>
#include <unordered_map>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/sendfile.h>

namespace server::client_handler::controllers {

ClientHandlerController::ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model,
    std::weak_ptr<view::IClientHandlerInterface> client_handler_interface,
    std::weak_ptr<context_handler::view::IContextHandlerInterface> context_handler_interface
)   : m_SERVER_STARTER_MODEL{server_model}, m_client_handler_interface{client_handler_interface.lock()}
    , m_context_handler_interface{context_handler_interface.lock()}
    , on_page_address_updated_slot{std::bind(&ClientHandlerController::on_page_address_updated, this, std::placeholders::_1)}   // TODO: depricated;
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

void ClientHandlerController::disconnect(std::weak_ptr<models::IClientHandlerModel> weak_client) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string());

    m_client_handler_interface->on_client_disconnected.emit(client->socket());

    m_client_handler_model_container.erase(client);
    
    if (client->socket().close()) {
        BLOG_ERROR("Cannot to close the client!");
    }
}

void ClientHandlerController::read_data(std::weak_ptr<models::IClientHandlerModel> weak_client) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string());

    while (client->socket().is_valid()) {
        BLOG_DEBUG("Reading client: ", client->socket().to_string());
        std::string read_buf;
        auto bytes_read = client->socket().read(read_buf, common::config::BUFFER_SIZE);
        handle_read(client, std::move(read_buf), bytes_read);
    }
}

void ClientHandlerController::send_data(std::weak_ptr<models::IClientHandlerModel> weak_client) {

}

void ClientHandlerController::handle_connect(const common::Socket& client_socket) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = std::make_shared<models::ClientHandlerModel>();
    client->set_socket(std::move(client_socket));

    BLOG_INFO("client: ", client->socket().to_string());

    m_client_handler_model_container.insert(client);

    std::thread(&ClientHandlerController::read_data, this, client).detach();
}

void ClientHandlerController::handle_read(std::weak_ptr<models::IClientHandlerModel> weak_client, std::string&& read_data, std::int32_t bytes_read) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string(), "bytes_read = ", bytes_read, "; data = ", read_data);

    if (bytes_read > 0) {
        common::HttpHeaders http_headers(read_data);
        handle_http_request(client, http_headers);
    } else if (bytes_read == 0) {
        BLOG_INFO("Disconnect client", client->socket().to_string());
        disconnect(client);
        // TODO: create signal-slot for disconnect client;
    } else {
        BLOG_ERROR("Error in reading data: ", client->socket().latest_error());
    }
}

void ClientHandlerController::handle_http_request(std::weak_ptr<models::IClientHandlerModel> weak_client, const common::HttpHeaders &header) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string(), "; http request type = ", common::EnumStringConvertor::init()->to_string(header.method), "; http body = ", header.body);
    // m_context_handler_interface->request_to_open_uri(header.uri);

    // int fdimg = open(image_path.c_str(), O_RDONLY);

    m_context_handler_interface->request_to_open_uri("/", client->socket());
    
}

}   // !server::client_handler::controllers;