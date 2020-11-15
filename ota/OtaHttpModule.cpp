#include "OtaHttpModule.h"

namespace EBLi::ota {

OtaHttpModule::OtaHttpModule(OtaUpdater *otaUpdater) : m_otaUpdater(otaUpdater)
{}

std::vector<http::module::HttpModule::HttpEndpoint> OtaHttpModule::getHttpEndpoints() const
{
    auto postOtaHandler = [=](httpd_req_t *request) {
        return handlePost(request, [=](cJSON *jsonRequestData, cJSON **jsonResponseData) -> bool {
            cJSON *const otaUrl = cJSON_GetObjectItem(jsonRequestData, "url");
            if (!otaUrl || !cJSON_IsString(otaUrl) || strlen(otaUrl->valuestring) <= 4) {
                return false;
            }

            if (!m_otaUpdater) {
                return false;
            }
            m_otaUpdater->setUpdateUrl(otaUrl->valuestring);

            *jsonResponseData = cJSON_CreateObject();
            if (m_otaUpdater->startUpdate()) {
                cJSON_AddStringToObject(*jsonResponseData, "msg", "update started");
                return true;
            } else {
                cJSON_AddStringToObject(*jsonResponseData, "msg", "update failed");
                cJSON_AddNumberToObject(*jsonResponseData, "code", m_otaUpdater->getLastError());
                return false;
            }
        });
    };

    return {
        HttpEndpoint {
            .method = HTTP_POST,
            .uri = "/ota",
            .handler = postOtaHandler,
        },
    };
}

}
