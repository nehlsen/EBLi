#ifndef EBLI_FILESYSTEM_H
#define EBLI_FILESYSTEM_H

#include "HttpModule.h"

namespace EBLi::http::module {

class FileSystem : public HttpModule
{
public:
    FileSystem();

    [[nodiscard]] std::vector<httpd_uri_t *> getHandlers() override;

private:
    httpd_uri_t m_filesystem_uri;
    static esp_err_t getFileSystemHttpHandler(httpd_req_t *request);
};

}

#endif //EBLI_FILESYSTEM_H
