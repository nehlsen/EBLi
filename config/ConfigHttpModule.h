#ifndef EBLI_CONFIGHTTPMODULE_H
#define EBLI_CONFIGHTTPMODULE_H

#include <HttpModule.h>

namespace EBLi::config {

class ConfigHttpModule: public http::module::HttpModule
{
public:
    [[nodiscard]] std::vector<HttpEndpoint> getHttpEndpoints() const override;
};

}

#endif //EBLI_CONFIGHTTPMODULE_H
