set(COMPONENT_SRCS  "webserver.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES nvs_flash protocol_examples_common esp_http_server powerCtrl
                    )