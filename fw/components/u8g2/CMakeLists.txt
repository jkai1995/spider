file(GLOB_RECURSE SOURCES csrc/*.c)
set(COMPONENT_SRCS  "u8g2_esp32_hal.c")

idf_component_register(SRCS "${SOURCES}" "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
					PRIV_REQUIRES i2cDriver
                    )