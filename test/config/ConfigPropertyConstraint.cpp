#include "unity.h"
#include <ConfigPropertyConstraint.h>
#include <string>

#define TEST_ASSERT_EQUAL_STD_STRING(expected, actual) TEST_ASSERT_EQUAL_STRING(expected, actual.c_str())

TEST_CASE("Config: Constraint Number", "[EBLi][Config]")
{
    auto numberConstraint = EBLi::ConfigPropertyConstraint::Number(1, 1);
    TEST_ASSERT_EQUAL(1, numberConstraint->constrain(1));
    TEST_ASSERT_EQUAL(1, numberConstraint->constrain(2));
    TEST_ASSERT_EQUAL(1, numberConstraint->constrain(-1));
    delete numberConstraint;
    
    numberConstraint = EBLi::ConfigPropertyConstraint::Number(0, 10);
    TEST_ASSERT_EQUAL(0, numberConstraint->constrain(0));
    TEST_ASSERT_EQUAL(10, numberConstraint->constrain(11));
    TEST_ASSERT_EQUAL(0, numberConstraint->constrain(-1));
    delete numberConstraint;

    numberConstraint = EBLi::ConfigPropertyConstraint::Number(10, 0);
    TEST_ASSERT_EQUAL(0, numberConstraint->constrain(0));
    TEST_ASSERT_EQUAL(10, numberConstraint->constrain(11));
    TEST_ASSERT_EQUAL(0, numberConstraint->constrain(-1));
    delete numberConstraint;

    numberConstraint = EBLi::ConfigPropertyConstraint::Number(-10, -5);
    TEST_ASSERT_EQUAL(-5, numberConstraint->constrain(0));
    TEST_ASSERT_EQUAL(-5, numberConstraint->constrain(11));
    TEST_ASSERT_EQUAL(-10, numberConstraint->constrain(-11));
    TEST_ASSERT_EQUAL(-7, numberConstraint->constrain(-7));
    delete numberConstraint;
}

TEST_CASE("Config: Constraint StringLength", "[EBLi][Config]")
{
    auto stringLengthConstraint = EBLi::ConfigPropertyConstraint::StringLength(1);
    TEST_ASSERT_EQUAL_STD_STRING("a", stringLengthConstraint->constrain(std::string("a")));
    TEST_ASSERT_EQUAL_STD_STRING("a", stringLengthConstraint->constrain(std::string("ab")));
    TEST_ASSERT_EQUAL_STD_STRING("", stringLengthConstraint->constrain(std::string()));
    delete stringLengthConstraint;

    stringLengthConstraint = EBLi::ConfigPropertyConstraint::StringLength(10);
    TEST_ASSERT_EQUAL_STD_STRING("", stringLengthConstraint->constrain(std::string()));
    TEST_ASSERT_EQUAL_STD_STRING("abcdefghij", stringLengthConstraint->constrain(std::string("abcdefghijNOTIN")));
    delete stringLengthConstraint;

    stringLengthConstraint = EBLi::ConfigPropertyConstraint::StringLength(0);
    TEST_ASSERT_EQUAL_STD_STRING("", stringLengthConstraint->constrain(std::string()));
    TEST_ASSERT_EQUAL_STD_STRING("", stringLengthConstraint->constrain(std::string("NOTIN")));
    delete stringLengthConstraint;

    stringLengthConstraint = EBLi::ConfigPropertyConstraint::StringLength(-10);
    TEST_ASSERT_EQUAL_STD_STRING("", stringLengthConstraint->constrain(std::string()));
    TEST_ASSERT_EQUAL_STD_STRING("", stringLengthConstraint->constrain(std::string("NOTIN")));
    delete stringLengthConstraint;
}
