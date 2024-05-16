#ifndef POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_CLIENTHANDLERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_CLIENTHANDLERMODEL_HPP

#include "IClientHandlerModel.hpp"

#include <atomic>
#include <mutex>

namespace server::client_handler::models {
    
class ClientHandlerModel : public IClientHandlerModel {
public:
    common::Socket socket(void) const override;
    void set_socket(const common::Socket& val) override;
    void set_socket(common::Socket&& val) override;

private:
    mutable std::mutex m_socket_mutex;
    common::Socket m_socket;
};

}   // !server::client_handler::models;

#endif  // !POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_MODELS_CLIENTHANDLERMODEL_HPP;