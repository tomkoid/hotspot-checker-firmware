set(srcs "main.c" "task.c" "touchpad.c" "url.c" "wifi.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "")
