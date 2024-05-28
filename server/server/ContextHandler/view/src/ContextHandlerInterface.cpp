#include "ContextHandlerInterface.hpp"

#include "Logger.hpp"

namespace server::context_handler::view {

void ContextHandlerInterface::request_to_open_uri(std::string uri) {
    BDECLARE_TAG_SCOPE("ContextHandlerInterface", __FUNCTION__);

    BLOG_VERBOSE("uri = ", uri);

    open_uri.emit(uri);
}

}   // !server::context_handler::view;