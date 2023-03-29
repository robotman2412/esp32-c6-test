idf_component_register(
    SRCS
        "main.cpp"
    INCLUDE_DIRS
        "."
        "elfloader/src"
    EMBED_FILES
        "../app/test1/build/main.o"
)

add_subdirectory(elfloader)
target_link_libraries(${COMPONENT_LIB} PUBLIC elfloader)
