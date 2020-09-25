set(TEST_SRCS
        config/ConfigProperty.cpp
        config/ConfigPropertyConstraint.cpp
        )

idf_component_register(
        SRCS ${TEST_SRCS}
        INCLUDE_DIRS "."
        REQUIRES unity EBLi
)
