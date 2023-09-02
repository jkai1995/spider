set(COMPONENT_SRCS  "sdCtrl.c" "sdio.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
					PRIV_REQUIRES gpioDriver fatfs
					)