#include "unity.h"
#include <string>
#include <ConfigProperty.h>
#include <ConfigPropertyConstraint.h>

// Mock Config Manager
namespace EBLi {
class ConfigManager
{
public:
    static ConfigProperty *createProperty(const std::string &shortKey, const std::string &longKey = std::string())
    {
        return new ConfigProperty(shortKey, longKey);
    }
};
}

TEST_CASE("Config: Property", "[EBLi][Config]")
{
    auto property = EBLi::ConfigManager::createProperty("foo");
    TEST_ASSERT_EQUAL_STRING("foo", property->getShortKey().c_str());
    TEST_ASSERT_EQUAL_STRING("foo", property->getLongKey().c_str());

    EBLi::ConfigProperty *fluent = nullptr;
    TEST_ASSERT_EQUAL(0, property->getDefaultValue<int>());
    fluent = property->setDefaultValue(1234);
    TEST_ASSERT(fluent == property);
    TEST_ASSERT_EQUAL(1234, property->getDefaultValue<int>());
    fluent = nullptr;

    TEST_ASSERT_EQUAL_STRING("", property->getDefaultValue<std::string>().c_str());
    fluent = property->setDefaultValue("some default");
    TEST_ASSERT(fluent == property);
    TEST_ASSERT_EQUAL_STRING("some default", property->getDefaultValue<std::string>().c_str());
    fluent = nullptr;

    TEST_ASSERT_EQUAL(0, property->getValue<int>());
    property->setValue(1234);
    TEST_ASSERT_EQUAL(1234, property->getValue<int>());

    TEST_ASSERT_EQUAL_STRING("", property->getValue<std::string>().c_str());
    property->setValue("some value");
    TEST_ASSERT_EQUAL_STRING("some value", property->getValue<std::string>().c_str());
}

TEST_CASE("Config: Property - long key", "[EBLi][Config]")
{
    auto property = EBLi::ConfigManager::createProperty("foo", "bar");
    TEST_ASSERT_EQUAL_STRING("foo", property->getShortKey().c_str());
    TEST_ASSERT_EQUAL_STRING("bar", property->getLongKey().c_str());
}

TEST_CASE("Config: Property - set value with constraint", "[EBLi][Config]")
{
    auto property = EBLi::ConfigManager::createProperty("foo");

    EBLi::ConfigProperty *fluent = nullptr;
    fluent = property->setConstraint(EBLi::ConfigPropertyConstraint::Number(2, 4));
    TEST_ASSERT(fluent == property);

    property->setValue(1);
    TEST_ASSERT_EQUAL(2, property->getValue<int>());
    property->setValue(2);
    TEST_ASSERT_EQUAL(2, property->getValue<int>());
    property->setValue(5);
    TEST_ASSERT_EQUAL(4, property->getValue<int>());
}

TEST_CASE("Config: Property - change handler is called and has new value", "[EBLi][Config]")
{
    auto property = EBLi::ConfigManager::createProperty("foo");

    bool changeHandlerWasCalled = false;
    property->setChangeHandler([&changeHandlerWasCalled](EBLi::ConfigProperty *property) {
        TEST_ASSERT_EQUAL(1, property->getValue<int>());
        changeHandlerWasCalled = true;
    });
    TEST_ASSERT_EQUAL(0, property->getValue<int>());
    property->setValue(1);
    TEST_ASSERT_EQUAL(1, property->getValue<int>());
    TEST_ASSERT_TRUE(changeHandlerWasCalled);
}
