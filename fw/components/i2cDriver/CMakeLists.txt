set(COMPONENT_SRCS  "i2cDriver.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
                    )