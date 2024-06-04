#include "ContextHandlerController.hpp"
#include "Logger.hpp"
#include "config.hpp"

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
    BLOG_INFO("uri = ", uri);

    m_context_handler_model->set_current_uri(uri);

    std::stringstream request;
    request << "HTTP/1.1 200 OK\r\n";
    std::string file_path = "./frontend_web";

    std::int32_t extension_index_pos = uri.find_last_of(common::config::EXTENSTION_SEPARATOR);
    std::string extension = "html";
    if (extension_index_pos != std::string::npos && extension_index_pos < uri.size()) {
        extension = uri.substr(extension_index_pos + 1);
        BLOG_DEBUG("take extension from ", extension_index_pos + 1, "; size = ", uri.size());
    }

    if (extension == "html") {
        request << "Content-Type: text/html\r\n";
        file_path += uri + common::config::DEFAULT_HTML_FILE;
    } else if (extension == "css") {
        request << "Content-Type: text/css\r\n";
        file_path += uri;
    } else if (extension == "jpg" || extension == "jpeg") {
        request << "Content-Type: image/jpeg\r\n";
        file_path += uri;
    } else if (extension == "png") {
        request << "Content-Type: image/png\r\n";
        file_path += uri;
    } else if (extension == "svg") {
        request << "Content-Type: image/svg+xml\r\n";
        file_path += uri;
    } else if (extension.find("wof") != std::string::npos) {
        request << "Content-Type: font/woff\r\n";
        file_path += uri;
    } else if (extension == "ico") {
        request << "Content-Type: image/vnd.microsoft.icon\r\n";
        file_path += uri;
    } else {
        request << "Content-Type: application/octet-stream\r\n";
        file_path += uri;
    }

    BLOG_DEBUG("request = ", request.str(), "; file_path = ", file_path);

    int fdimg = open(file_path.c_str(), O_RDONLY);
    if (fdimg < 0) {
        BLOG_ERROR("Cannot open file path: ", file_path, ". Error: ", strerror(errno));
        std::string error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
        ::write(socket.m_socket_fd, error_response.c_str(), error_response.size());
        return;
    }

    struct stat stat_buf;
    if (fstat(fdimg, &stat_buf) == -1) {
        BLOG_ERROR("Cannot get file stats: ", strerror(errno));
        close(fdimg);
        return;
    }

    off_t offset = 0;
    ssize_t sent_bytes = 0;

    request << "Content-Length: " << stat_buf.st_size << "\r\n";
    request << "Connection: close\r\n\r\n";

    std::string response = request.str();
    if (::write(socket.m_socket_fd, response.c_str(), response.size()) == -1) {
        BLOG_ERROR("Failed to write HTTP headers: ", strerror(errno));
        close(fdimg);
        return;
    }

    while (offset < stat_buf.st_size) {
        sent_bytes = ::sendfile(socket.m_socket_fd, fdimg, &offset, stat_buf.st_size - offset);
        if (sent_bytes <= 0) {
            BLOG_ERROR("Error sending file: ", strerror(errno));
            break;
        }
    }

    if (sent_bytes > 0) {
        BLOG_INFO("Sent file: ", file_path);
    }

    // m_context_handler_interface->page_updated.emit(uri, socket);

    close(fdimg);
}

} // namespace server::context_handler::controllers
