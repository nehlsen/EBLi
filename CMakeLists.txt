set(EBLi_SRCS core/ebli.cpp)
set(EBLi_DEPENDENCIES)

if(CONFIG_ENABLE_EBLI_FS)
    set(EBLi_SRCS ${EBLi_SRCS} fs/FS.cpp)
#    set(EBLi_DEPENDENCIES ${EBLi_DEPENDENCIES} spiffs nvs_flash)
    list(APPEND EBLi_DEPENDENCIES spiffs nvs_flash)
endif()

if(CONFIG_ENABLE_EBLI_WIFI)
    set(EBLi_SRCS ${EBLi_SRCS} wifi/Wifi.cpp wifi/Wifip.cpp)
#    set(EBLi_DEPENDENCIES ${EBLi_DEPENDENCIES} wifi_provisioning)
    list(APPEND EBLi_DEPENDENCIES wifi_provisioning)
endif()

idf_component_register(
        SRCS ${EBLi_SRCS}
        INCLUDE_DIRS core
        PRIV_INCLUDE_DIRS fs wifi
        PRIV_REQUIRES spiffs nvs_flash wifi_provisioning
#        PRIV_REQUIRES ${EBLi_DEPENDENCIES}
)
