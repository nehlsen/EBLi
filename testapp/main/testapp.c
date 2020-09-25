#include <stdio.h>
#include "unity.h"

void app_main(void)
{
    printf("Running tests with [EBLi] tag");
    UNITY_BEGIN();
    unity_run_tests_by_tag("[EBLi]", false);
//    unity_run_tests_by_tag("[Config]", false);
    UNITY_END();
}
