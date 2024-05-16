#ifndef POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_SERVERSTARTERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_SERVERSTARTERMODEL_HPP

#include "IServerStarterModel.hpp"

#include <mutex>

namespace server::serverstarter::models {

class ServerStarterModel : public IServerStarterModel {
public:
    common::Socket socket(void) const override;
    void set_socket(const common::Socket& val) override;
    void set_socket(common::Socket&& val) override;

private:
    mutable std::mutex m_socket_mutex;
    common::Socket m_socket;
};

}   // !server::serverstarter::models;

#endif  // !POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_SERVERSTARTERMODEL_HPP;