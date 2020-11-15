#ifndef EBLI_WEBSERVERP_H
#define EBLI_WEBSERVERP_H

#include <esp_http_server.h>
#include "HttpModule.h"

namespace EBLi::http {

class WebServerp
{
public:
    WebServerp();

    void startServer();
    void stopServer();

    [[nodiscard]] bool isServerRunning() const;

    void addModule(const module::HttpModule &httpModule);

    static esp_err_t genericHttpHandler(httpd_req_t *request);

private:
    httpd_handle_t m_hndlServer = nullptr;
    void registerUriHandlers();

    std::vector<module::HttpModule::HttpEndpoint> m_httpEndpoints;
    module::HttpModule::HttpEndpoint* findHttpEndpoint(http_method method, const char *uri);
};

}

#endif //EBLI_WEBSERVERP_H
