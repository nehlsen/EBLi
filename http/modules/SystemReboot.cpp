#include "SystemReboot.h"

namespace EBLi::http::module {

std::vector<HttpModule::HttpEndpoint> SystemReboot::getHttpEndpoints() const
{
    auto systemRebootHandler = [](httpd_req_t *request) {
//        ESP_LOGI(LOG_TAG, "SYSTEM REBOOT TRIGGERED!");

        esp_restart(); // TODO via PowerManager ?

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
