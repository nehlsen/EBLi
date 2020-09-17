set(EBLi_SRCS core/ebli.cpp)
set(EBLi_PUBLIC_INCLUDE_DIRS core/public)
set(EBLi_DEPENDENCIES)

if(CONFIG_ENABLE_EBLI_FS)
#    set(EBLi_SRCS ${EBLi_SRCS} fs/FS.cpp)
    list(APPEND EBLi_SRCS fs/FS.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS fs/public)
#    set(EBLi_DEPENDENCIES ${EBLi_DEPENDENCIES} spiffs nvs_flash)
    list(APPEND EBLi_DEPENDENCIES spiffs nvs_flash)
endif()

if(CONFIG_ENABLE_EBLI_WIFI)
#    set(EBLi_SRCS ${EBLi_SRCS} wifi/Wifi.cpp wifi/Wifip.cpp)
    list(APPEND EBLi_SRCS wifi/Wifi.cpp wifi/Wifip.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS wifi/public)
    #    set(EBLi_DEPENDENCIES ${EBLi_DEPENDENCIES} wifi_provisioning)
    list(APPEND EBLi_DEPENDENCIES wifi_provisioning)
endif()

if(CONFIG_ENABLE_EBLI_MQTT)
    #    set(EBLi_SRCS ${EBLi_SRCS} mqtt/Mqtt.cpp)
    list(APPEND EBLi_SRCS mqtt/Mqtt.cpp mqtt/Mqttp.cpp mqtt/MqttSubscriber.cpp mqtt/MqttPublisher.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS mqtt/public)
    #    set(EBLi_DEPENDENCIES ${EBLi_DEPENDENCIES} wifi_provisioning)
    list(APPEND EBLi_DEPENDENCIES mqtt)
endif()

if(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    #    set(EBLi_SRCS ${EBLi_SRCS} mqtt/Mqtt.cpp)
    list(APPEND EBLi_SRCS config/ConfigManager.cpp)
    list(APPEND EBLi_PUBLIC_INCLUDE_DIRS config/public)
    #    set(EBLi_DEPENDENCIES ${EBLi_DEPENDENCIES} wifi_provisioning)
#    list(APPEND EBLi_DEPENDENCIES mqtt)
endif()

idf_component_register(
        SRCS ${EBLi_SRCS}
        INCLUDE_DIRS ${EBLi_PUBLIC_INCLUDE_DIRS}
        PRIV_INCLUDE_DIRS fs wifi
        PRIV_REQUIRES spiffs nvs_flash wifi_provisioning mqtt
#        PRIV_REQUIRES ${EBLi_DEPENDENCIES}
)

# if FS
# spiffs_create_partition_image(data ${DATA_DIR} FLASH_IN_PROJECT)