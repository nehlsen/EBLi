#ifndef EBLI_SYSTEMINFO_H
#define EBLI_SYSTEMINFO_H

#include "HttpModule.h"

namespace EBLi::http::module {

class SystemInfo : public HttpModule
{
public:
    [[nodiscard]] std::vector<HttpEndpoint> getHttpEndpoints() const override;

private:
    static void responseAddChipInfo(cJSON *responseObject);
    static void responseAddLastResetReason(cJSON *responseObject);
    static void responseAddRestartCounter(cJSON *responseObject);
    static void responseAddOtaInfo(cJSON *responseObject);
    static void responseAddEbliInfo(cJSON *responseObject);
};

}

#endif //EBLI_SYSTEMINFO_H
