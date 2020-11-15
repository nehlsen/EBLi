#ifndef EBLI_SYSTEMREBOOT_H
#define EBLI_SYSTEMREBOOT_H

#include "HttpModule.h"

namespace EBLi::http::module {

class SystemReboot : public HttpModule
{
public:
    [[nodiscard]] std::vector<HttpEndpoint> getHttpEndpoints() const override;
};

}

#endif //EBLI_SYSTEMREBOOT_H
