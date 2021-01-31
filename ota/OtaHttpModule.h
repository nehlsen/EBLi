#ifndef EBLI_OTA_HTTPMODULE_H
#define EBLI_OTA_HTTPMODULE_H

#include <HttpModule.h>
#include <OtaUpdater.h>

namespace EBLi::ota {

class OtaHttpModule: public http::module::HttpModule
{
public:
    explicit OtaHttpModule(OtaUpdater *otaUpdater);

    [[nodiscard]] std::vector<httpd_uri_t *> getHandlers() override;

private:
    OtaUpdater *m_otaUpdater = nullptr;

    httpd_uri_t m_ota_uri;
    static esp_err_t postOtaHttpHandler(httpd_req_t *request);
};

}

#endif //EBLI_OTA_HTTPMODULE_H
