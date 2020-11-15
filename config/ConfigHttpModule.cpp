#include "ConfigHttpModule.h"
#include <ConfigManager.h>

namespace EBLi::config {

std::vector<http::module::HttpModule::HttpEndpoint> ConfigHttpModule::getHttpEndpoints() const
{
    auto getConfigHandler = [](httpd_req_t *request) {
        auto cm = ConfigManager::instance();
        return jsonResponse(cm->toJson(), request);
    };

    auto postConfigHandler = [](httpd_req_t *request) {
        return handlePost(request, [](cJSON *jsonRequestData, cJSON **jsonResponseData) {
            auto cm = ConfigManager::instance();
            bool hasChanged = cm->fromJson(jsonRequestData);
            *jsonResponseData = cm->toJson();
            return hasChanged;
        });
    };

    return {
        HttpEndpoint {
            .method = HTTP_GET,
            .uri = "/config",
            .handler = getConfigHandler,
        },
        HttpEndpoint {
            .method = HTTP_POST,
            .uri = "/config",
            .handler = postConfigHandler,
        },
    };
}

}
