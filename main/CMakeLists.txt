idf_component_register(
    SRCS
        "main.cpp"
    INCLUDE_DIRS
        "."
    EMBED_FILES
        # "../app/test7/build/main7.o"
        "../app/test6/build/main6.o"
        "../lib/pax-graphics/build/libpax.so"
)
