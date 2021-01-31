#ifndef LEDWALL_HTTPMODULE_H
#define LEDWALL_HTTPMODULE_H

#include <esp_http_server.h>
#include <functional>
#include <cJSON.h>

#define BASE_URI "/api/v2" //!< no trailing "/" !

namespace EBLi::http::module {

class HttpModule
{
public:
    [[nodiscard]] virtual std::vector<httpd_uri_t*> getHandlers() = 0;

    [[nodiscard]] bool isRegistered() const;
    void setRegistered(bool isRegistered);

protected:
    /**
     * !!! takes ownership of and deletes the JSON Object
     */
    static esp_err_t sendJsonResponse(cJSON *jsonData, httpd_req_t *request, const char* httpStatus = "200");
    [[deprecated("by sendJsonResponse")]] static esp_err_t jsonResponse(cJSON *root, httpd_req_t *req, const char* status = "200");

    static esp_err_t jsonRequestHelper(httpd_req_t *request, const std::function<bool(cJSON *jsonRequestData, cJSON **jsonResponseData)>& jsonHandler);
    [[deprecated("by jsonRequestHelper")]] static esp_err_t handlePost(httpd_req_t *req, const std::function<bool(cJSON *request, cJSON **response)>& jsonHandler);

private:
    bool m_isRegistered = false;
};

}

#endif //LEDWALL_HTTPMODULE_H
