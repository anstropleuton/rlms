set(RLMS_MINESWEEPER_SOURCES
    "rlms.cpp"
)

add_library(rlms_lib ${RLMS_MINESWEEPER_SOURCES})
target_compile_features(rlms_lib PUBLIC cxx_std_23)
target_include_directories(rlms_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set(RLMS_EXE_SOURCES
    "main.cpp"
    "rlmsg.cpp"
)

find_package(raylib REQUIRED)

add_executable(rlms ${RLMS_EXE_SOURCES})
target_link_libraries(rlms PRIVATE rlms_lib raylib)
