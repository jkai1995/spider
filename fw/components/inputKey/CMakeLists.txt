set(COMPONENT_SRCS  "inputKey.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
					PRIV_REQUIRES gpioDriver
					)