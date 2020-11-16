#include "WebServerp.h"
#include <ebli_log.h>
#include <esp_event.h>

namespace EBLi::http {

#define BASE_URI "/api/v2" //!< no trailing "/" !

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
    config.max_uri_handlers = 2;
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
    // TODO check if we have same method+uri already registered...

    m_httpModules.push_back(httpModule);
}

esp_err_t WebServerp::genericHttpHandler(httpd_req_t *request)
{
    char methodString[5];
    switch (request->method) {
        case HTTP_GET: sprintf(methodString, "GET"); break;
        case HTTP_POST: sprintf(methodString, "POST"); break;
        default: sprintf(methodString, "%d", request->method); break;
    }
    ESP_LOGI(LOG_TAG_HTTP, "genericHttpHandler: [%s] '%s'", methodString, request->uri);

    auto *server = static_cast<WebServerp*>(request->user_ctx);
    if (!server) {
        return ESP_FAIL;
    }

    auto endpoint = server->findHttpEndpoint(static_cast<http_method>(request->method), request->uri);
    if (nullptr == endpoint) {
        return httpd_resp_send_err(request, HTTPD_404_NOT_FOUND, nullptr);
    }

    if (endpoint->handler(request) != ESP_OK) {
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, nullptr);
    }

    return ESP_OK;
}

void WebServerp::registerUriHandlers()
{
    ESP_LOGI(LOG_TAG_HTTP, "Registering URI handlers");

    httpd_uri_t system_info_get_uri = {
            .uri = BASE_URI"/*",
            .method = HTTP_GET,
            .handler = genericHttpHandler,
            .user_ctx = this
    };
    httpd_register_uri_handler(m_hndlServer, &system_info_get_uri);

    httpd_uri_t system_reboot_post_uri = {
            .uri = BASE_URI"/*",
            .method = HTTP_POST,
            .handler = genericHttpHandler,
            .user_ctx = this
    };
    httpd_register_uri_handler(m_hndlServer, &system_reboot_post_uri);

    // TODO default index, e.g. redirect "/" (or "/index.html") to e.g. "/fs/index.html"
    //   or a special "fallback" handler for files?!
}

module::HttpModule::HttpEndpoint* WebServerp::findHttpEndpoint(http_method method, const char *uri)
{
    for (auto &module : m_httpModules) {
        for (auto &endpoint : module->getHttpEndpoints()) {
            if (endpoint.method != method) {
                continue;
            }

            char checkUri[64];
            sprintf(checkUri, "%s%s", BASE_URI, endpoint.uri);
            if (!httpd_uri_match_wildcard(checkUri, uri, strlen(uri))) {
                continue;
            }

            return &endpoint;
        }
    }

    return nullptr;
}

}
