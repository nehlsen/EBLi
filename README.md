# EBLi - ESP32 Base Library

> I was implementing similar base functionalities in all my ESP32 Projects and was beginning to duplicate lots of code. So I needed some kind of _base lib_.

This grew... alot! Now _EBLi_ is more like a Framework on top of _IDF_.

Features include
- _my preferred WiFi Setup_ including provisioning via BLE and auto start of provisioning if configured WiFi can not be connected
- Configuration Management using _Properties_ and a _Config-Manager_
- HTTP and MQTT accessibility including auto wiring for Configuration Management (and other properties)
- OTA which can be triggered via HTTP and MQTT
- Sensors regularly reads _sensors_ and publishes the values via MQTT. A Battery Sensor measures battery voltage via an ADC pin. The I2C temperature & humidity Sensor AHT10 is supported too.
- Status uses an LED connected to one of the GPIOs to report system status using different _blink codes_
- Time synchronization using NTP

## Core

All required sub-components can be enabled via `make menuconfig` or via `sdkconfig.defaults`. Use `EBLi::init_all()` to initialize all active sub-components at once.
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

### Events

_EBLi_ reports some info via events. See `core/public/ebli_events.h` for all available Events.
The events are mostly used to tell _Status_ about the current overall status.

## FS

_Initialize NVS and SPIFFS_\
Filesystems will be formatted if not readable.

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

## Config

Mainly provides the `ConfigManager` to read and write config-properties. A `ConfigProperty` contains either a `std::string` or an `int`. Each `ConfigProperty` has a short-key and a long-key. The short-key is used for storage and can not be longer than 15 chars. The long-key is used for MQTT and HTTP. Values are persisted to NVS and will be restored on power-up/reboot.

* If `http` is enabled, all config properties can automatically be read and written via HTTP-API.
  * `GET` `<host>/api/v2/config` \
    Get current config as JSON Document
  * `POST` `<host>/api/v2/config`
    Set/Update config. Use the JSON Document returned by _get config_ and modify appropriately. Missing keys remain unchanged, unknown keys are ignored, some value validation is included - but not much.
* If `mqtt` is enabled, all config properties can automatically be read and written via MQTT.

Sample Config Property
```c++
auto configManager = EBLi::ConfigManager::instance();
auto myProperty = configManager->property("my_prop", "MyProperty");
myProperty->setDefaultValue(42);

// leads 42 (except this is not the first boot)
int val = myProperty->getValue<int>();

// this value will be written to disc. If you reboot, the above line will read 17 instead of 42
myProperty->setValue(17);

// leads 17
val = myProperty->getValue<int>();
```

### Property Visibility and Accessibility

Properties can have different _Visibility_ and _Accessibility_. HTTP and MQTT use these flags to filter the properties or make them read-only.

#### MQTT

| Visibility | Accessibility | Subscribe     | Publish |
|------------|---------------|---------------|---------|
| Hidden     |  ReadOnly     | None          | None    |
| Hidden     |  ReadWrite    | None          | None    |
| Device     |  ReadOnly     | None          | Device  |
| Device     |  ReadWrite    | Device        | Device  |
| Group      |  ReadOnly     | None          | Device  |
| Group      |  ReadWrite    | Device, Group | Device  |

#### HTTP

| Visibility | Accessibility | get      | set      |
|------------|---------------|----------|----------|
| Hidden     |  ReadOnly     | excluded | ignored  |
| Hidden     |  ReadWrite    | excluded | accepted |
| Device     |  ReadOnly     | included | ignored  |
| Device     |  ReadWrite    | included | accepted |
| Group      |  ReadOnly     | included | ignored  |
| Group      |  ReadWrite    | included | accepted |

## HTTP

Provides an HTTP Web-Server. _Endpoints_ can be added at runtime.

Following _Endpoints_ are registered by default
* SystemInfo \
  allows to get some system info using a `GET` to `<host>/api/v2/system/info`. System info is a JSON Document
* SystemReboot \
  allows to reboot the system using a `POST` to `<host>/api/v2/system/reboot`
* Filesystem \
  allows to access the _"Filesystem"_ via `GET` of `<host>/api/v2/fs/*`. The `*` has to be replaced by a _known_ filename. Directory listing is not supported.


## MQTT

* TODO publish is prefixed with...
* TODO subscribe is prefixed with...
* TODO device topic, group topic

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

## OTA

### Trigger an update programmatically

```c++
auto otaUpdater = new EBLi::OtaUpdater;
otaUpdater->setUpdateUrl("http://my-update.server/path/to/my/update.bin");
if (otaUpdater->startUpdate()) {
    std::cout << "Update started";
} else {
    std::cout << "Failed to start update: " << otaUpdater->getLastError();
}
```

### Trigger an update via HTTP-API

If `http` is enabled an appropriate http handler is automatically registered

```http request
POST http://<my-device>/api/v2/ota
Content-Type: application/json

{
  "url": "http://my-update.server/path/to/my/update.bin"
}
```

### Trigger an update via MQTT

If `mqtt` is enabled an appropriate subscription is automatically setup.

```shell script
mosquitto_pub -h my-mqtt-broker -t 'myDevice/cmd/ota' -m 'http://my-update.server/path/to/my/update.bin'
```

Some info about the update can be acquired using the topic `myDevice/state/ota`.

```shell script
mosquitto_sub -h my-mqtt-broker -t 'myDevice/state/ota'
```

## Sensors

## Status

Most ESP32 Boards have an LED on-board which can be freely used. Status uses this LED (or any other LED connected to one of the GPIOs) to report system status using different _blink codes_.

Blink-Codes

Blinks | Meaning
-------|--------
5x fast, 750ms pause | WiFi disconnected
2x fast, 750ms pause | WiFi connecting
500ms continuous | WiFi provisioning active
1s on, 1s off | WiFi connected but MQTT-Broker disconnected
1x fast, 2s pause | WiFi and MQTT-Broker connected

## Test & Testapp

_TODO_

## Time

If enabled, this module will synchronize the time via NTP using the `pool.ntp.org` NTP Server. The timezone will be set to Central-Europe by default. 

## Wifi

If this module is enabled, Wifi and Wifi-Provisioning will be initialized and started.

* if already provisioned, Wifi will be connected and provisioning stopped
  * if connection fails, provisioning mode will be started
* if NOT already provisioned, provisioning will be started

When provisioning is active, the Wifi can be provisioned via Bluetooth using the Espressif sample app
* Android: https://play.google.com/store/apps/details?id=com.espressif.provble 
* iOS: https://apps.apple.com/in/app/esp-ble-provisioning/id1473590141

The default _Proof of Possession_ Secret is `abcd1234`.

## Utility

- LedBlinker can be used to blink an LED with different _blink_ patterns

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

CONFIG_EBLI_FS_ENABLE=y
CONFIG_EBLI_WIFI_ENABLE=y
CONFIG_EBLI_MQTT_ENABLE=y
CONFIG_EBLI_MQTT_DEFAULT_BROKER="my-mqtt-broker.lan"
CONFIG_EBLI_MQTT_DEFAULT_DEVICE_TOPIC="my-cool-device"
CONFIG_EBLI_MQTT_DEFAULT_GROUP_TOPIC="all-my-devices"
#CONFIG_EBLI_TIME_ENABLE=y
CONFIG_EBLI_OTA_ENABLE=y
CONFIG_EBLI_CONFIG_MANAGER_ENABLE=y
```
