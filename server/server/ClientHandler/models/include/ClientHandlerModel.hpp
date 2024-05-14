#ifndef POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_CLIENTHANDLERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_CLIENTHANDLERMODEL_HPP

#include "IClientHandlerModel.hpp"

#include <atomic>

namespace server::client_handler::models {
    
class ClientHandlerModel : public IClientHandlerModel {
public:
    std::int32_t socket_fd(void) const noexcept override;
    void set_socket_fd(std::int32_t val) noexcept override;

private:
    std::atomic<std::int32_t> m_socket_fd;
};

}   // !server::client_handler::models;

#endif  // !POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_CLIENTHANDLERMODEL_HPP;