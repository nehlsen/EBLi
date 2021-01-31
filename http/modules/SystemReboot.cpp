#include "SystemReboot.h"

namespace EBLi::http::module {

SystemReboot::SystemReboot():
    m_system_reboot_uri {
        .uri = BASE_URI "/system/reboot",
        .method = http_method::HTTP_POST,
        .handler = SystemReboot::postSystemRebootHttpHandler,
        .user_ctx = nullptr
    }
{}

std::vector<httpd_uri_t *> SystemReboot::getHandlers()
{
    return {&m_system_reboot_uri};
}

esp_err_t SystemReboot::postSystemRebootHttpHandler(httpd_req_t *request)
{
    httpd_resp_send(request, "", 0);
    esp_restart();
    return ESP_OK;
}

}
