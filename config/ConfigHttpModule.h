#ifndef EBLI_CONFIGHTTPMODULE_H
#define EBLI_CONFIGHTTPMODULE_H

#include <HttpModule.h>

namespace EBLi::config {

class ConfigHttpModule: public http::module::HttpModule
{
public:
    ConfigHttpModule();

    [[nodiscard]] std::vector<httpd_uri_t *> getHandlers() override;

private:
    httpd_uri_t m_get_config_uri;
    static esp_err_t getConfigHttpHandler(httpd_req_t *request);
    httpd_uri_t m_post_config_uri;
    static esp_err_t postConfigHttpHandler(httpd_req_t *request);
};

}

#endif //EBLI_CONFIGHTTPMODULE_H
