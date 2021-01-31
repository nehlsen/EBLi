#include "OtaHttpModule.h"

namespace EBLi::ota {

OtaHttpModule::OtaHttpModule(OtaUpdater *otaUpdater):
    m_otaUpdater(otaUpdater),
    m_ota_uri {
        .uri = BASE_URI "/ota",
        .method = http_method::HTTP_POST,
        .handler = OtaHttpModule::postOtaHttpHandler,
        .user_ctx = m_otaUpdater
    }
{}

std::vector<httpd_uri_t *> OtaHttpModule::getHandlers()
{
    return {&m_ota_uri};
}

esp_err_t OtaHttpModule::postOtaHttpHandler(httpd_req_t *request)
{
    auto otaUpdater = static_cast<OtaUpdater*>(request->user_ctx);
    if (nullptr == otaUpdater) {
        return ESP_FAIL;
    }

    return handlePost(request, [otaUpdater](cJSON *jsonRequestData, cJSON **jsonResponseData) -> bool {
        cJSON *const otaUrl = cJSON_GetObjectItem(jsonRequestData, "url");
        if (!otaUrl || !cJSON_IsString(otaUrl) || strlen(otaUrl->valuestring) <= 4) {
            return false;
        }

        if (!otaUpdater) {
            return false;
        }
        otaUpdater->setUpdateUrl(otaUrl->valuestring);

        *jsonResponseData = cJSON_CreateObject();
        if (otaUpdater->startUpdate()) {
            cJSON_AddStringToObject(*jsonResponseData, "msg", "update started");
            return true;
        } else {
            cJSON_AddStringToObject(*jsonResponseData, "msg", "update failed");
            cJSON_AddNumberToObject(*jsonResponseData, "code", otaUpdater->getLastError());
            return false;
        }
    });
}

}
