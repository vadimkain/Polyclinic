#include "ContextHandlerController.hpp"
#include "Logger.hpp"
#include "config.hpp"
#include "HttpHeaders.hpp"
#include "File.hpp"

#include <sys/stat.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sstream>
#include <fcntl.h>
#include <cstring>

namespace server::context_handler::controllers {

ContextHandlerController::ContextHandlerController(std::weak_ptr<models::IContextHandlerModel> model,
                                                   std::weak_ptr<view::IContextHandlerInterface> view)
    : m_context_handler_model{model.lock()}
    , m_context_handler_interface{view.lock()}
    , on_open_uri_slot{std::bind(&ContextHandlerController::on_open_uri, this, std::placeholders::_1, std::placeholders::_2)}
{
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("Constructor called");

    init();
}

void ContextHandlerController::init() {
    common::connect(&m_context_handler_interface->open_uri, &on_open_uri_slot);
}

void ContextHandlerController::on_open_uri(std::string uri, common::Socket socket) {
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("uri = ", uri, "; socket = ", socket.to_string());

    m_context_handler_model->set_current_uri(uri);

    std::stringstream request;
    request << "HTTP/1.1 200 OK\r\n";
    std::string file_path = "./frontend_web";

    std::int32_t extension_index_pos = uri.find_last_of(common::config::EXTENSTION_SEPARATOR);
    std::string extension = "html";

    if (extension_index_pos != std::string::npos && extension_index_pos < uri.size()) {
        extension = uri.substr(extension_index_pos + 1);
    }

    common::HttpHeaders http_header;

    request << http_header.extension_to_content_type(extension);
    file_path += uri;
    if (extension == "html" && extension_index_pos == std::string::npos) {
        file_path += common::config::DEFAULT_HTML_FILE;
    }

    BLOG_DEBUG("request = ", request.str(), "; file_path = ", file_path);

    common::File requested_file;

    if (requested_file.open(file_path, common::File::OpenTypeEnum::READ_ONLY) < 0) {
        BLOG_ERROR("Cannot open file path: ", file_path, ". Error: ", requested_file.latest_error());
        std::string error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
        socket.send(error_response);

        requested_file.close();
        return;
    }

    request << "Content-Length: " << requested_file.size() << "\r\n";
    request << "Connection: close\r\n\r\n";

    std::string response = request.str();
    if (socket.send(response) < 0) {
        BLOG_ERROR("Failed to write HTTP headers: ", strerror(errno));
        requested_file.close();
        return;
    }

    auto sent_bytes = socket.sendfile(requested_file);

    if (sent_bytes == requested_file.size()) {
        BLOG_DEBUG("Sent file: ", file_path, " happened successfull. Was sent ", sent_bytes, " bytes.");
    } else {
        BLOG_ERROR("Error sending file. Bytes sent = ", sent_bytes, "; total size = ", requested_file.size(), ". Error ", socket.latest_error());
    }

    requested_file.close();

    m_context_handler_interface->page_updated.emit(uri, socket);
}

} // namespace server::context_handler::controllers
