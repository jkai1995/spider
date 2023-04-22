set(COMPONENT_SRCS  "main.c")

idf_component_register(SRCS "${COMPONENT_SRCS}"
                    INCLUDE_DIRS "./")
