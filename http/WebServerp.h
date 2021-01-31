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

    void addModule(module::HttpModule *httpModule);

private:
    httpd_handle_t m_hndlServer = nullptr;
    void registerUriHandlers();

    std::vector<module::HttpModule*> m_httpModules;

    void registerModuleHandlers(module::HttpModule *module);
};

}

#endif //EBLI_WEBSERVERP_H
