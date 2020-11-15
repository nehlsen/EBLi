#ifndef EBLI_OTA_HTTPMODULE_H
#define EBLI_OTA_HTTPMODULE_H

#include <HttpModule.h>
#include <OtaUpdater.h>

namespace EBLi::ota {

class OtaHttpModule: public http::module::HttpModule
{
public:
    explicit OtaHttpModule(OtaUpdater *otaUpdater);

    [[nodiscard]] std::vector<HttpEndpoint> getHttpEndpoints() const override;

private:
    OtaUpdater *m_otaUpdater = nullptr;
};

}

#endif //EBLI_OTA_HTTPMODULE_H
