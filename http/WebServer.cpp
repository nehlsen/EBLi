#include "public/WebServer.h"
#include "WebServerp.h"
#include "modules/SystemInfo.h"
#include "modules/SystemReboot.h"
#include "modules/FileSystem.h"

namespace EBLi::http {

WebServer* WebServer::s_instance = nullptr;

WebServer* WebServer::instance()
{
    if (s_instance == nullptr) {
        s_instance = new WebServer;
    }

    return s_instance;
}

void WebServer::init()
{
    auto srv = instance();
    srv->addModule(new module::SystemInfo());
    srv->addModule(new module::SystemReboot());
    srv->addModule(new module::FileSystem());
}

WebServer::WebServer():
    m_webServerp(new WebServerp)
{
}

void WebServer::startServer()
{
    m_webServerp->startServer();
}

void WebServer::stopServer()
{
    m_webServerp->stopServer();
}

bool WebServer::isServerRunning() const
{
    return m_webServerp->isServerRunning();
}

void WebServer::addModule(module::HttpModule *httpModule)
{
    m_webServerp->addModule(httpModule);
}

}
