#include "ContextHandlerInterface.hpp"

#include "Logger.hpp"

namespace server::context_handler::view {

void ContextHandlerInterface::request_to_open_uri(std::string uri, common::Socket socket) {
    BDECLARE_TAG_SCOPE("ContextHandlerInterface", __FUNCTION__);

    BLOG_VERBOSE("uri = ", uri);

    open_uri.emit(uri, socket);
}

}   // !server::context_handler::view;