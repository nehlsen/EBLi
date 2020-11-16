#include "SystemReboot.h"

namespace EBLi::http::module {

std::vector<HttpModule::HttpEndpoint> SystemReboot::getHttpEndpoints() const
{
    auto systemRebootHandler = [](httpd_req_t *request) {
        httpd_resp_send(request, "", 0);
        esp_restart();
        return ESP_OK;
    };

    return {
        HttpEndpoint {
                .method = HTTP_POST,
                .uri = "/system/reboot",
                .handler = systemRebootHandler,
        },
    };
}

}
