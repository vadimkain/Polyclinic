#ifndef POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_ICLIENTHANDLERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_ICLIENTHANDLERMODEL_HPP

#include <cinttypes>

namespace server::client_handler::models {
    
class IClientHandlerModel {
public:
    virtual std::int32_t socket_fd(void) const noexcept = 0;
    virtual void set_socket_fd(std::int32_t val) noexcept = 0;
};

}   // !server::client_handler::models;

#endif  // !POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_ICLIENTHANDLERMODEL_HPP;