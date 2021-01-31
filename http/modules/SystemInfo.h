#ifndef EBLI_SYSTEMINFO_H
#define EBLI_SYSTEMINFO_H

#include "HttpModule.h"

namespace EBLi::http::module {

class SystemInfo : public HttpModule
{
public:
    SystemInfo();

    [[nodiscard]] std::vector<httpd_uri_t *> getHandlers() override;

private:
    httpd_uri_t m_system_info_uri;
    static esp_err_t getSystemInfoHttpHandler(httpd_req_t *request);

    static void responseAddChipInfo(cJSON *responseObject);
    static void responseAddLastResetReason(cJSON *responseObject);
    static void responseAddRestartCounter(cJSON *responseObject);
    static void responseAddOtaInfo(cJSON *responseObject);
    static void responseAddEbliInfo(cJSON *responseObject);
};

}

#endif //EBLI_SYSTEMINFO_H
