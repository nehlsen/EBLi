#include "Status.h"
#include <ebli_log.h>
#include <esp_event.h>
#include <LedBlinker.h>
#include <ebli_events.h>

#define LED_GPIO GPIO_NUM_2

namespace EBLi::Status {

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto status = static_cast<Status*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == status ? ESP_FAIL : ESP_OK);

    status->setWifiConnected(true);
}

Status::Status()
{
    m_ledBlinker = new Utility::LedBlinker(LED_GPIO);
    updateBlinker();

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(EBLI_EVENTS, ESP_EVENT_ANY_ID, on_ebli_event, this));
}

Status::State Status::getState() const
{
    if (m_wifiConnected) {
        return WIFI_CONNECTED;
    }

    return NOT_CONNECTED;
}

void Status::setWifiConnected(bool isConnected)
{
    m_wifiConnected = isConnected;
}

}
