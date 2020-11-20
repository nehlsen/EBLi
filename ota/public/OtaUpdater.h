#ifndef EBLI_OTAUPDATER_H
#define EBLI_OTAUPDATER_H

#include <esp_err.h>

namespace EBLi::ota {

class OtaUpdaterPrivate;

class OtaUpdater
{
public:
    OtaUpdater();

    void setUpdateUrl(const char* url);
    bool startUpdate();

    esp_err_t getLastError() const;

protected:
    OtaUpdaterPrivate *m_updater;
};

}

#endif //EBLI_OTAUPDATER_H
