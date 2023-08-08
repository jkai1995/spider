set(COMPONENT_SRCS  "oled.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
					PRIV_REQUIRES i2cDriver
                    )