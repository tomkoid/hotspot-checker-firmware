set(srcs "main.c" "task.c" "wifi.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "")
