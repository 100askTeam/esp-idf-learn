set(srcs "epd_100ask_simple_test_main.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")
