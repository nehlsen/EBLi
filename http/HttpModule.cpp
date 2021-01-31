#include "../public/HttpModule.h"

namespace EBLi::http::module {

bool HttpModule::isRegistered() const
{
    return m_isRegistered;
}

void HttpModule::setRegistered(bool isRegistered)
{
    m_isRegistered = isRegistered;
}

esp_err_t HttpModule::sendJsonResponse(cJSON *jsonData, httpd_req_t *request, const char *httpStatus)
{
    if (jsonData == nullptr) {
        httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Error");
        return ESP_FAIL;
    }

    httpd_resp_set_status(request, httpStatus);
    httpd_resp_set_type(request, "application/json");

    const char *responsePayload = cJSON_PrintUnformatted(jsonData);
    httpd_resp_sendstr(request, responsePayload);

    free((void *)responsePayload);
    cJSON_Delete(jsonData);

    return ESP_OK;
}

esp_err_t HttpModule::jsonResponse(cJSON *root, httpd_req_t *req, const char *status)
{
    return sendJsonResponse(root, req, status);
}

esp_err_t HttpModule::jsonRequestHelper(httpd_req_t *request, const std::function<bool(cJSON *, cJSON **)> &jsonHandler)
{
    const int readBufferSize = 5 * 1024;

    const int total_len = request->content_len;
    if (total_len >= readBufferSize) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }

    int cur_len = 0;
    char *readBuffer = static_cast<char *>(calloc(readBufferSize, sizeof(char)));
    while (cur_len < total_len) {
        int received = httpd_req_recv(request, readBuffer + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    readBuffer[total_len] = '\0';

    cJSON *jsonRequestData = cJSON_Parse(readBuffer);
    free(readBuffer);

    if (!jsonRequestData) {
        httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "Failed to parse JSON");
        return ESP_FAIL;
    }

    cJSON *jsonResponseData = nullptr;
    if (!jsonHandler(jsonRequestData, &jsonResponseData)) {
        if (jsonResponseData == nullptr) {
            httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "JSON is missing keys or has invalid values");
        } else {
            sendJsonResponse(jsonResponseData, request, "400 Bad Request");
        }
        cJSON_Delete(jsonRequestData);
        return ESP_FAIL;
    }

    cJSON_Delete(jsonRequestData);
    return sendJsonResponse(jsonResponseData, request);
}

esp_err_t HttpModule::handlePost(httpd_req_t *req, const std::function<bool(cJSON *, cJSON **)> &jsonHandler)
{
    return jsonRequestHelper(req, jsonHandler);
}

}
