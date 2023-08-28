set(COMPONENT_SRCS  "powerCtrl.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
					PRIV_REQUIRES esp_adc_cal
					)