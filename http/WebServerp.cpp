#include "WebServerp.h"
#include <ebli_log.h>
#include <esp_event.h>

namespace EBLi::http {

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto webServerp = static_cast<WebServerp*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == webServerp ? ESP_FAIL : ESP_OK);

    webServerp->startServer();
}

WebServerp::WebServerp()
{
    ESP_LOGI(LOG_TAG_HTTP, "Creating server, waiting for IP to start...");

    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, this);
}

void WebServerp::startServer()
{
    if (isServerRunning()) {
        ESP_LOGE(LOG_TAG_HTTP, "NOT starting WebServer, already running...");
        return;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // FIXME check
//    config.stack_size = 5 * 1024;
    config.max_uri_handlers = 19;
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(LOG_TAG_HTTP, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&m_hndlServer, &config) == ESP_OK) {
        registerUriHandlers();
    } else {
        ESP_LOGE(LOG_TAG_HTTP, "Failed to start WebServer");
    }
}

void WebServerp::stopServer()
{
    httpd_stop(m_hndlServer);
}

bool WebServerp::isServerRunning() const
{
    return m_hndlServer != nullptr;
}

void WebServerp::addModule(module::HttpModule *httpModule)
{
    m_httpModules.push_back(httpModule);

    if (isServerRunning()) {
        registerModuleHandlers(httpModule);
    }
}

void WebServerp::registerUriHandlers()
{
    ESP_LOGI(LOG_TAG_HTTP, "Registering URI handlers");

    for (auto &module : m_httpModules) {
        registerModuleHandlers(module);
    }
}

void WebServerp::registerModuleHandlers(module::HttpModule *module)
{
    if (module->isRegistered()) {
        return;
    }

    for (auto &handler : module->getHandlers()) {
        ESP_LOGI(LOG_TAG_HTTP, "Registering Module handler: %s '%s'", http_method_str(handler->method), handler->uri);
        auto registerResult = httpd_register_uri_handler(m_hndlServer, handler);
        switch (registerResult) {
            case ESP_OK:
                ESP_LOGD(LOG_TAG_HTTP, "Module handler registered successfully");
                module->setRegistered(true);
                break;

            case ESP_ERR_INVALID_ARG:
                ESP_LOGE(LOG_TAG_HTTP, "Failed to register Module handler: Invalid Arguments");
                break;

            case ESP_ERR_HTTPD_HANDLERS_FULL:
                ESP_LOGE(LOG_TAG_HTTP, "Failed to register Module handler: Too much handlers");
                break;

            case ESP_ERR_HTTPD_HANDLER_EXISTS:
                ESP_LOGE(LOG_TAG_HTTP, "Failed to register Module handler: Same Method/URI already registered");
                break;
        }
    }
}

}
