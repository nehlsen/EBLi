#ifndef EBLI_WIFIP_H
#define EBLI_WIFIP_H

#include <esp_err.h>
#include <wifi_provisioning/manager.h>

namespace EBLi
{

class Wifip
{
public:
    static esp_err_t init();

    static esp_err_t provisionWifi();
    static void startProvisioning();
    static void prov_service_name(char *service_name, size_t max);
    static void startWifiStation();
    static void countdownAndRestart(uint8_t countdown);

    static void provisioning_event_handler(void *user_data, wifi_prov_cb_event_t event, void *event_data);
    static void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
};

}

#endif //EBLI_WIFIP_H
