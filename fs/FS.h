#ifndef EBLI_FS_H
#define EBLI_FS_H

#include <esp_err.h>
#include <esp_spiffs.h>

namespace EBLi
{

class FS
{
public:
    static esp_err_t init();

private:
    static bool m_isInitialized;

    static esp_err_t initNvs();
    static esp_err_t initSpiffs();
    static esp_vfs_spiffs_conf_t createSpiffsConfig();
    static void logFsInfo(const char* partition_label = nullptr);
};

}

#endif //EBLI_FS_H
