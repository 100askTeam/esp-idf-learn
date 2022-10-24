set(srcs "epd_100ask_lvgl_main.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")
