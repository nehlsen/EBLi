#ifndef EBLI_WEBSERVER_H
#define EBLI_WEBSERVER_H

#include "HttpModule.h"

namespace EBLi::http {

class WebServerp;
class WebServer
{
public:
    static WebServer *instance();
    static void init();

    void startServer();
    void stopServer();

    [[nodiscard]] bool isServerRunning() const;

    void addModule(module::HttpModule *httpModule);

private:
    static WebServer *s_instance;
    WebServer();

    WebServerp *m_webServerp;
};

}

#endif //EBLI_WEBSERVER_H
