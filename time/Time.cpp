#include "Time.h"
#include <esp_log.h>
#include <esp_event.h>
#include <esp_sntp.h>
#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
#include <ConfigManager.h>
#endif

#define DEFAULT_TIMEZONE "CET-1CEST,M3.5.0/2,M10.5.0/2"

namespace EBLi
{

static const char *LOG_TAG = "EBLi:Time";

bool Time::m_isInitialized = false;

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}

esp_err_t Time::init()
{
    if (m_isInitialized) {
        ESP_LOGW(LOG_TAG, "already initialized...");
        return ESP_OK;
        // maybe? e.g. esp_event_loop_create_default() does the same...
//        return ESP_ERR_INVALID_STATE;
    }

    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, nullptr);

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    initConfig();
#endif

    m_isInitialized = true;

    return ESP_OK;
}

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
void Time::initConfig()
{
    auto updateTimezone = [](ConfigProperty *property) {
        setenv("TZ", property->getValue<std::string>().c_str(), 1);
        tzset();
    };
    auto timezoneConfig = ConfigManager::instance()
        ->property("timezone")
        ->setDefaultValue(DEFAULT_TIMEZONE)
        ->setChangeHandler(updateTimezone);
    updateTimezone(timezoneConfig);
}
#endif

}
