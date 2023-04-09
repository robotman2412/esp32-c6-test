idf_component_register(
    SRCS
        "main.cpp"
        "ecall.s"
    INCLUDE_DIRS
        "."
        "elfloader/src"
    EMBED_FILES
        "../app/test2/build/main.o"
        "../lib/test1/build/libtest.so"
)

add_subdirectory(elfloader)
target_link_libraries(${COMPONENT_LIB} PUBLIC elfloader)
