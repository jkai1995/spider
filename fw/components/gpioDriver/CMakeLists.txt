set(COMPONENT_SRCS  "gpioDriver.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
                    )