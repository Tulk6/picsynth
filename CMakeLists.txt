# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import.cmake)

project(picsynth C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(picsynth picsynth.c )

pico_set_program_name(picsynth "picsynth")
pico_set_program_version(picsynth "0.1")

# Generate PIO header
#pico_generate_pio_header(picsynth ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(picsynth 0)
pico_enable_stdio_usb(picsynth 1)

# Add the standard library to the build
target_link_libraries(picsynth
        pico_stdlib
        pico_audio_i2s
        hardware_adc
        hardware_i2c
        pico_rand)

target_compile_definitions(picsynth PRIVATE
    # compile time configuration of I2S
    PICO_AUDIO_I2S_MONO_INPUT=1
    PICO_AUDIO_I2S_DATA_PIN=20
    PICO_AUDIO_I2S_CLOCK_PIN_BASE=18
    PICO
)

# Add the standard include files to the build
target_include_directories(picsynth PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries

pico_add_extra_outputs(picsynth)

