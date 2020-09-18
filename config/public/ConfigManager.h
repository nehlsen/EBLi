#ifndef EBLI_CONFIGMANAGER_H
#define EBLI_CONFIGMANAGER_H

#include <nvs.h>
#include <string>

namespace EBLi {

class ConfigManager
{
public:
    static ConfigManager *init();
    static ConfigManager *instance();

    bool open();
    bool isOpen() const;
    void close();
    bool commit();

//    enum ConfigOptions {
//        NoOptions,
//        AutoPublishMqtt,   // publish change of value via MQTT "deviceTopic/config/KEY"
//        AutoSubscribeMqtt, // allow change of value via MQTT "deviceTopic/config/KEY" and  "groupTopic/config/KEY"
//        // TODO same for REST
//    };
//    void createConfig(const std::string &key, options);

    void setAutoCommitEnabled(bool enabled = true);
    bool isAutoCommitEnabled() const;

    int32_t getValue(const std::string &key, int32_t defaultValue) const;
    bool setValue(const std::string &key, int32_t value);

    std::string getValue(const std::string &key, const std::string &defaultValue) const;
    bool setValue(const std::string &key, const std::string &value);

    int32_t getRestartCounter();

private:
    ConfigManager() = default;
    static ConfigManager *s_instance;

    bool openNvs();
    void logNvsState();
    bool m_isOpen = false;

    static bool isKeyValid(const std::string &key);
    static bool checkKeyAndLog(const std::string &key);

    bool m_autoCommitEnabled = true;
    bool autoCommit();

    nvs_handle_t m_nvsHandle = 0;

    void updateRestartCounter();
    int32_t m_restartCounter = 0;
};

}

#endif //EBLI_CONFIGMANAGER_H
