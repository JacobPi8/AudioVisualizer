cmake_minimum_required(VERSION 3.30)
project(Audio_Visualizer_FFT)

set(CMAKE_CXX_STANDARD 20)

set(SDL3_DIR "${CMAKE_SOURCE_DIR}/library/SDL3/cmake")

find_package(SDL3 REQUIRED CONFIG)

add_executable(Audio-Visualizer-FFT
        app/src/main.cpp
        app/src/application.cpp
        app/src/audio_engine.cpp
        app/src/visualizer.cpp
        app/include/application.h
        app/include/audio_engine.h
        app/include/visualizer.h
)

target_include_directories(Audio-Visualizer-FFT PRIVATE app/include)
target_link_libraries(Audio-Visualizer-FFT PRIVATE SDL3::SDL3)
target_compile_definitions(Audio-Visualizer-FFT PRIVATE SDL_MAIN_HANDLED)

add_custom_command(TARGET Audio-Visualizer-FFT POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/library/SDL3/lib/x64/SDL3.dll"
        $<TARGET_FILE_DIR:Audio-Visualizer-FFT>
        COMMENT "Copying SDL3.dll to output directory"
)
