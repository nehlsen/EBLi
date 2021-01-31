#ifndef EBLI_SYSTEMREBOOT_H
#define EBLI_SYSTEMREBOOT_H

#include "HttpModule.h"

namespace EBLi::http::module {

class SystemReboot : public HttpModule
{
public:
    SystemReboot();

    [[nodiscard]] std::vector<httpd_uri_t *> getHandlers() override;

private:
    httpd_uri_t m_system_reboot_uri;
    static esp_err_t postSystemRebootHttpHandler(httpd_req_t *request);
};

}

#endif //EBLI_SYSTEMREBOOT_H
