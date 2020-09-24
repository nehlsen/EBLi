# EBLi - ESP32 Base Library

> I was implementing similar base functionalities in all my ESP32 Projects and was beginning to duplicate lots of code. So I needed some kind of _base lib_.

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

# NOTES

- ~~wifi provisioning~~
- config manager
  - WebConfig
- expose config over mqtt
- ~~common MQTT~~
- common REST
- OTA

targets i have in mind
- LedWall
- OneWheel Monitor
- Flowers

---


## TODO 

- ~~move public includes to separate folder(s) and mark them es exported (CMakeLists)~~
