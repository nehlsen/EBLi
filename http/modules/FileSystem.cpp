#include "FileSystem.h"
#include <esp_http_server.h>
#include <esp_vfs.h>
#include <ebli_log.h>
#include <fcntl.h>
#include <esp_ota_ops.h>

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define READ_BUFFER_SIZE (1024 * 5)

#define LOG_TAG LOG_TAG_HTTP ":ModFs"
#define FS_BASE_URI "/fs" //!< no trailing "/" !

namespace EBLi::http::module {

class FileResponseHandler
{
public:
    static esp_err_t handleRequest(httpd_req_t *request);

protected:
    explicit FileResponseHandler(httpd_req_t *request);

    esp_err_t doIt();

    void determineRequestedFile();
    void setMimeType();
    esp_err_t sendFile();

    httpd_req_t *m_request;

    char m_requestedFile[FILE_PATH_MAX];
    int m_fd;
};

FileSystem::FileSystem():
    m_filesystem_uri {
        .uri = BASE_URI FS_BASE_URI "/*",
        .method = http_method::HTTP_GET,
        .handler = FileSystem::getFileSystemHttpHandler,
        .user_ctx = nullptr
    }
{}

std::vector<httpd_uri_t *> FileSystem::getHandlers()
{
    return {&m_filesystem_uri};
}

esp_err_t FileSystem::getFileSystemHttpHandler(httpd_req_t *request)
{
    return FileResponseHandler::handleRequest(request);
}

/**********************************************************************************************************************/

esp_err_t FileResponseHandler::handleRequest(httpd_req_t *request)
{
    return (new FileResponseHandler(request))->doIt();
}

FileResponseHandler::FileResponseHandler(httpd_req_t *request):
    m_request(request), m_requestedFile(""), m_fd(0)
{}

esp_err_t FileResponseHandler::doIt()
{
    determineRequestedFile();
    setMimeType();

    m_fd = open(m_requestedFile, O_RDONLY, 0);
    if (m_fd == -1) {
        ESP_LOGE(LOG_TAG, "Failed to open file : %s", m_requestedFile);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(m_request, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    sendFile();

    /* Close file after sending complete */
    close(m_fd);
    ESP_LOGI(LOG_TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(m_request, nullptr, 0);

    return ESP_OK;
}

void FileResponseHandler::determineRequestedFile()
{
    // this produces "/index.html" no matter what directory is accessed TODO
    //   a) if uri is "/" this is ok
    //   b) else a 404 or denies would be more appropriate
    if (m_request->uri[strlen(m_request->uri) - 1] == '/') {
        strlcat(m_requestedFile, "/index.html", sizeof(m_requestedFile));
    } else {
        char *filename = strstr(m_request->uri, FS_BASE_URI"/");
        if (nullptr != filename) {
            filename += strlen(FS_BASE_URI); // keep trailing slash to be leading slash of filename

            strlcat(m_requestedFile, filename, sizeof(m_requestedFile));
        } else {
            strlcat(m_requestedFile, m_request->uri, sizeof(m_requestedFile));
        }
    }

    ESP_LOGI(LOG_TAG, "Requested File: \"%s\"", m_requestedFile);
}

void FileResponseHandler::setMimeType()
{
    auto isFileExtension = [=](const char *ext)->bool {
        return strcmp(&m_requestedFile[strlen(m_requestedFile) - strlen(ext)], ext) == 0;
    };

    const char *type = "text/plain";
    if (isFileExtension(".html")) {
        type = "text/html";
    } else if (isFileExtension(".js")) {
        type = "application/javascript";
    } else if (isFileExtension(".css")) {
        type = "text/css";
    } else if (isFileExtension(".png")) {
        type = "image/png";
    } else if (isFileExtension(".ico")) {
        type = "image/x-icon";
    } else if (isFileExtension(".svg")) {
        type = "text/xml";
    } else if (isFileExtension(".json")) {
        type = "application/json";
    }
    httpd_resp_set_type(m_request, type);

    ESP_LOGI(LOG_TAG, "Requested File Mimetype: %s", type);
}

esp_err_t FileResponseHandler::sendFile()
{
    char *readBuffer = (char*)calloc(READ_BUFFER_SIZE, sizeof(char));
    ssize_t read_bytes;

    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(m_fd, readBuffer, READ_BUFFER_SIZE);
        if (read_bytes == -1) {
            ESP_LOGE(LOG_TAG, "Failed to read file : %s", m_requestedFile);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(m_request, readBuffer, read_bytes) != ESP_OK) {
                close(m_fd);
                ESP_LOGE(LOG_TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(m_request, nullptr);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(m_request, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                free(readBuffer);
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);

    free(readBuffer);
    return ESP_OK;
}

}
