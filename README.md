# EBLi - ESP32 Base Library

> I was implementing similar base functionalities in all my ESP32 Projects and was beginning to duplicate lots of code. So I needed some kind of _base lib_.

## Core

Configure wanted sub-modules via `make menuconfig` or activate them via `sdkconfig.defaults`. Use `EBLi::init_all()` to initialize all active sub-components at once.
If for example `WiFi` is active, the provisioning will start or connect to WiFi if already provisioned.

Sample `main.cpp`
```c++
#include <ebli.h>

#ifdef __cplusplus
extern "C" {
#endif

void app_main()
{
    EBLi::init_all();
    
    // my awesome project starts here
}

#ifdef __cplusplus
} // extern "C"
#endif
```

## FS

For Filesystem to work, a custom partition table has to be used.

`sdkconfig.defaults`
```
### Required for FS to work
CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y
CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partition_table.csv"
```

`partition_table.csv`
```
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     ,        0x6000,
otadata,  data, ota,     ,        0x2000,
phy_init, data, phy,     ,        0x1000,
ota_0,    app,  ota_0,   ,        1900K,
ota_1,    app,  ota_1,   ,        1900K,
data,     data, spiffs,  ,        100K,
```

### Issues

If you get some Error relating to `data.bin` being not found \
try `make spiffs_data_bin`

## MQTT

**Publish**
```c++
auto mqtt = EBLi::Mqtt::instance();
auto myPublisher = mqtt->createPublisher("my_property");
myPublisher->publishValue("the value of my property");
```

**Subscribe**
```c++
auto mqtt = EBLi::Mqtt::instance();
mqtt->createSubscriber("my_property", [](const std::string &value) {
    std::cout << "Dang! 'my_property' has changed!" << std::endl;
    std::cout << "new value reads: '" << value << "'" << std::endl;
});
```

## Example `sdkconfig.defaults`

```
### Required for Wifi provisioning to work
CONFIG_BT_ENABLED=y
### Required for FS to work
CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y
CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partition_table.csv"

# Allow OTA over HTTP instead of only HTTPS
#CONFIG_OTA_ALLOW_HTTP=y

################################################################################
# EBLi Stuff

CONFIG_ENABLE_EBLI_FS=y
CONFIG_ENABLE_EBLI_WIFI=y
CONFIG_ENABLE_EBLI_MQTT=y
CONFIG_EBLI_MQTT_DEFAULT_BROKER="my-mqtt-broker.lan"
CONFIG_EBLI_MQTT_DEFAULT_DEVICE_TOPIC="my-cool-device"
CONFIG_EBLI_MQTT_DEFAULT_GROUP_TOPIC="all-my-devices"
#CONFIG_ENABLE_EBLI_TIME=y
CONFIG_ENABLE_EBLI_OTA=y
CONFIG_ENABLE_EBLI_CONFIG_MANAGER=y
```

# NOTES

- ~~wifi provisioning~~
- config manager
  - WebConfig
- ~~expose config over mqtt~~
- ~~common MQTT~~
- common REST
- ~~OTA~~

targets i have in mind
- LedWall
- OneWheel Monitor
- Flowers

---


## TODO 

- ~~move public includes to separate folder(s) and mark them es exported (CMakeLists)~~
