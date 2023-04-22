set(COMPONENT_SRCS  "webserver.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "include"
                    )