#ifndef EBLI_FILESYSTEM_H
#define EBLI_FILESYSTEM_H

#include "HttpModule.h"

namespace EBLi::http::module {

class FileSystem : public HttpModule
{
public:
    [[nodiscard]] std::vector<HttpEndpoint> getHttpEndpoints() const override;
};

}

#endif //EBLI_FILESYSTEM_H
