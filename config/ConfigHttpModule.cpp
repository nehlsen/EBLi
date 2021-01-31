#include "ConfigHttpModule.h"
#include <ConfigManager.h>

namespace EBLi::config {

ConfigHttpModule::ConfigHttpModule():
    m_get_config_uri {
        .uri = BASE_URI "/config",
        .method = http_method::HTTP_GET,
        .handler = ConfigHttpModule::getConfigHttpHandler,
        .user_ctx = nullptr
    },
    m_post_config_uri {
        .uri = BASE_URI "/config",
        .method = http_method::HTTP_POST,
        .handler = ConfigHttpModule::postConfigHttpHandler,
        .user_ctx = nullptr
    }
{}

std::vector<httpd_uri_t *> ConfigHttpModule::getHandlers()
{
    return {&m_get_config_uri, &m_post_config_uri};
}

esp_err_t ConfigHttpModule::getConfigHttpHandler(httpd_req_t *request)
{
    auto cm = ConfigManager::instance();
    return jsonResponse(cm->toJson(), request);
}

esp_err_t ConfigHttpModule::postConfigHttpHandler(httpd_req_t *request)
{
    return handlePost(request, [](cJSON *jsonRequestData, cJSON **jsonResponseData) {
        auto cm = ConfigManager::instance();
        bool hasChanged = cm->fromJson(jsonRequestData);
        *jsonResponseData = cm->toJson();
        return hasChanged;
    });
}

}
