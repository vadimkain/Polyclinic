#ifndef POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_ICLIENTHANDLERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_ICLIENTHANDLERMODEL_HPP

#include "Socket.hpp"

#include <cinttypes>
#include <netinet/in.h>

namespace server::client_handler::models {
    
class IClientHandlerModel {
public:
    virtual common::Socket socket(void) const = 0;
    virtual void set_socket(const common::Socket& val) = 0;
    virtual void set_socket(common::Socket&& val) = 0;
};

}   // !server::client_handler::models;

#endif  // !POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_ICLIENTHANDLERMODEL_HPP;