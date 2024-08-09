set(srcs "main.c" "task.c" "url.c" "wifi.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "")
