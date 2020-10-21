#include "Status.h"
#include <ebli_log.h>
#include <esp_event.h>
#include <Delayer.h>
#include <driver/gpio.h>

#define LED_GPIO GPIO_NUM_2
#define BASE_DELAY 1000
#define LOOP_DELAY 100

namespace EBLi::Status {

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto status = static_cast<Status*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == status ? ESP_FAIL : ESP_OK);

    status->setWifiConnected(true);
}

[[noreturn]] void status_task(void *pvParameter)
{
//    ESP_LOGI(LOG_TAG_STATUS, "Starting Status task...");
    auto status = static_cast<Status*>(pvParameter);
    ESP_ERROR_CHECK(nullptr == status ? ESP_FAIL : ESP_OK);

    Utility::Delayer delay(BASE_DELAY);
    bool ledIsOn = false;
    while (true) {
        if (status->getState() == Status::NOT_CONNECTED) {
            delay.setDelay(100);
        } else if (status->getState() == Status::WIFI_CONNECTED) {
            delay.setDelay(1000);
        } else if (status->getState() == Status::MQTT_CONNECTED) {
            delay.setDelay(5000);
        }
//        delay.setDelay(status->getState() * BASE_DELAY);

        if (delay.isTimedOut()) {
            ledIsOn = !ledIsOn;

            gpio_set_level(LED_GPIO, ledIsOn ? 1 : 0);
        }

        vTaskDelay(LOOP_DELAY / portTICK_PERIOD_MS);
    }
}

Status::Status()
{
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, this);

    gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    xTaskCreatePinnedToCore(
            &status_task,
            "status_task",
            1*1024,
            this,
            tskIDLE_PRIORITY,
            nullptr,
            0
    );
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
