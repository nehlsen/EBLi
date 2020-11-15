#ifndef LEDWALL_HTTPMODULE_H
#define LEDWALL_HTTPMODULE_H

#include <esp_http_server.h>
#include <functional>
#include <cJSON.h>

namespace EBLi::http::module {

class HttpModule
{
public:
    struct HttpEndpoint {
        const http_method method; //!< http-method, e.g. HTTP_GET (GET, POST, HEAD, ...)
        const char *uri; //!< uri to match, e.g. "/system/info". anything compatible with httpd_uri_match_wildcard(...)
        std::function<esp_err_t(httpd_req_t*)> handler; //!< handler to call when appropriate request arrives
    };

    [[nodiscard]] virtual std::vector<HttpEndpoint> getHttpEndpoints() const = 0;

protected:
    /**
     * !!! takes ownership of and deletes the JSON Object
     */
    static esp_err_t sendJsonResponse(cJSON *jsonData, httpd_req_t *request, const char* httpStatus = "200");
    [[deprecated("by sendJsonResponse")]] static esp_err_t jsonResponse(cJSON *root, httpd_req_t *req, const char* status = "200");

    static esp_err_t jsonRequestHelper(httpd_req_t *request, const std::function<bool(cJSON *jsonRequestData, cJSON **jsonResponseData)>& jsonHandler);
    [[deprecated("by jsonRequestHelper")]] static esp_err_t handlePost(httpd_req_t *req, const std::function<bool(cJSON *request, cJSON **response)>& jsonHandler);
};

}

#endif //LEDWALL_HTTPMODULE_H
