#ifndef EBLI_OTAUPDATERPRIVATE_H
#define EBLI_OTAUPDATERPRIVATE_H

#include <esp_https_ota.h>

namespace EBLi {

class OtaUpdaterPrivate
{
public:
    OtaUpdaterPrivate();

    bool isActive() const;

    esp_http_client_config_t httpConfig;
    esp_https_ota_config_t otaConfig;
    esp_https_ota_handle_t otaHandle = nullptr;

    esp_err_t lastError = ESP_OK;

    void work();
};

}

#endif //EBLI_OTAUPDATERPRIVATE_H
