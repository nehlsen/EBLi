set(EBLi_SRCS core/ebli.cpp)
set(EBLi_PUBLIC_INCLUDE_DIRS core/public)
set(EBLi_PRIV_INCLUDE_DIRS core)
set(EBLi_PRIV_REQUIRES)
set(EBLi_COMPONENTS_ENABLED)

### UTILITIES
list(APPEND EBLi_SRCS utility/Delayer.cpp utility/LedBlinker.cpp)
list(APPEND EBLi_PUBLIC_INCLUDE_DIRS utility/public)

if(CONFIG_EBLI_STATUS_ENABLE)
    list(APPEND EBLi_SRCS status/Status.cpp)
    list(APPEND EBLi_PRIV_INCLUDE_DIRS status)
    list(APPEND EBLi_COMPONENTS_ENABLED "status")
endif()

if(CONFIG_EBLI_FS_ENABLE)
    list(APPEND EBLi_SRCS fs/FS.cpp)
    list(APPEND EBLi_PRIV_INCLUDE_DIRS fs)
    list(APPEND EBLi_PRIV_REQUIRES spiffs nvs_flash)
    list(APPEND EBLi_COMPONENTS_ENABLED "fs")

    # TODO rename DATA_DIR to CONFIG_EBLI_FS_FLASH_DATA_DIR
    if(DATA_DIR)
        spiffs_create_partition_image(data ${DATA_DIR} FLASH_IN_PROJECT)
        message("++ EBLi: (legacy) SPIFFS Data directory '${DATA_DIR}'")
    elseif(CONFIG_EBLI_FS_FLASH_DATA_DIR)
        spiffs_create_partition_image(data ${CONFIG_EBLI_FS_FLASH_DATA_DIR} FLASH_IN_PROJECT)
        message("++ EBLi: SPIFFS Data directory '${CONFIG_EBLI_FS_FLASH_DATA_DIR}'")
    else()
        set(NO_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fs/nodata)
        message("++ EBLi: Do Data dir configured. using '${NO_DATA_DIR}' as substitute")
        spiffs_create_partition_image(data ${NO_DATA_DIR} FLASH_IN_PROJECT)
    endif()
endif()

if(CONFIG_EBLI_WIFI_ENABLE)
    list(APPEND EBLi_SRCS wifi/Wifi.cpp wifi/Wifip.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS wifi/public)
    list(APPEND EBLi_PRIV_INCLUDE_DIRS wifi)
    list(APPEND EBLi_PRIV_REQUIRES wifi_provisioning)
    list(APPEND EBLi_COMPONENTS_ENABLED "wifi")
endif()

if(CONFIG_EBLI_MQTT_ENABLE)
    list(APPEND EBLi_SRCS mqtt/Mqtt.cpp mqtt/Mqttp.cpp mqtt/MqttSubscriber.cpp mqtt/MqttPublisher.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS mqtt/public)
    list(APPEND EBLi_PRIV_REQUIRES mqtt json)
    list(APPEND EBLi_COMPONENTS_ENABLED "mqtt")
endif()

if(CONFIG_EBLI_HTTP_ENABLE)
    list(APPEND EBLi_SRCS http/WebServer.cpp http/WebServerp.cpp http/HttpModule.cpp)
    list(APPEND EBLi_SRCS http/modules/SystemInfo.cpp http/modules/SystemReboot.cpp http/modules/FileSystem.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS http/public)
    list(APPEND EBLi_PRIV_REQUIRES json esp_http_server)
    list(APPEND EBLi_COMPONENTS_ENABLED "http")
endif()

if(CONFIG_EBLI_OTA_ENABLE)
    list(APPEND EBLi_SRCS ota/OtaUpdater.cpp ota/OtaUpdaterPrivate.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS ota/public)
    list(APPEND EBLi_PRIV_REQUIRES esp_https_ota app_update)
    list(APPEND EBLi_COMPONENTS_ENABLED "ota")

    if(CONFIG_EBLI_MQTT_ENABLE)
        list(APPEND EBLi_SRCS ota/OtaMqttAdapter.cpp)
    endif()

    if(CONFIG_EBLI_HTTP_ENABLE)
        list(APPEND EBLi_SRCS ota/OtaHttpModule.cpp)
    endif()
endif()

if(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    list(APPEND EBLi_SRCS config/ConfigManager.cpp config/ConfigProperty.cpp config/ConfigPropertyConstraint.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS config/public)
    list(APPEND EBLi_COMPONENTS_ENABLED "config")

    if(CONFIG_EBLI_MQTT_ENABLE)
        list(APPEND EBLi_SRCS config/MqttBridge.cpp)
        list(APPEND EBLi_PRIV_INCLUDE_DIRS config)
    endif()

    if(CONFIG_EBLI_HTTP_ENABLE)
        list(APPEND EBLi_SRCS config/ConfigHttpModule.cpp)
    endif()
endif()

if(CONFIG_EBLI_TIME_ENABLE)
    list(APPEND EBLi_SRCS time/Time.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS time)
    list(APPEND EBLi_COMPONENTS_ENABLED "time")
endif()

if(CONFIG_EBLI_SENSORS_ENABLED)
    list(APPEND EBLi_SRCS sensors/Sensors.cpp)
    list(APPEND EBLi_PRIV_INCLUDE_DIRS sensors)
    list(APPEND EBLi_COMPONENTS_ENABLED "sensors")

    if(CONFIG_EBLI_SENSORS_AHT10_ENABLED)
        list(APPEND EBLi_PRIV_REQUIRES AHT10-IDF)
    endif()

    if(CONFIG_EBLI_SENSORS_BATTERY_ENABLED)
        list(APPEND EBLi_SRCS sensors/Battery.cpp)
        list(APPEND EBLi_PRIV_REQUIRES esp_adc_cal)
    endif()
endif()

idf_component_register(
        SRCS ${EBLi_SRCS}
        INCLUDE_DIRS ${EBLi_PUBLIC_INCLUDE_DIRS}
        PRIV_INCLUDE_DIRS ${EBLi_PRIV_INCLUDE_DIRS}
#        PRIV_REQUIRES ${EBLi_PRIV_REQUIRES}
        PRIV_REQUIRES "spiffs;nvs_flash;wifi_provisioning;mqtt;json;esp_http_server;esp_https_ota;app_update;AHT10-IDF;esp_adc_cal"
)

list(JOIN EBLi_COMPONENTS_ENABLED ", " EBLi_COMPONENTS_ENABLED_OUT)
message("++ EBLi: enabled sub-components: ${EBLi_COMPONENTS_ENABLED_OUT}")

execute_process(
        COMMAND git describe --exact-match --tags
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE EBLi_VERSION_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE EBLi_VERSION_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
message("++ EBLi Version: ${EBLi_VERSION_TAG} ${EBLi_VERSION_HASH}")
configure_file(core/public/ebli_version.h.in ${CMAKE_BINARY_DIR}/generated/ebli_version.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)
