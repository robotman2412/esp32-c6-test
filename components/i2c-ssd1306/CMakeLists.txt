    set(srcs
        "driver_ssd1306.c"
    )
    set(includes
        "include"
    )

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ${includes} REQUIRES driver bus-i2c)
