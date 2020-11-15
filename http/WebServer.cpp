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
    srv->addModule(module::SystemInfo());
    srv->addModule(module::SystemReboot());
    srv->addModule(module::FileSystem());
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

void WebServer::addModule(const module::HttpModule &httpModule)
{
    m_webServerp->addModule(httpModule);
}

}
