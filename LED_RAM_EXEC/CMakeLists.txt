# This file was created to explore and capture the process of how Keil's compiler organise the whole project and how the Keil's settings lead the compiler's work

# Global variables
set(ROOT /workspace/LED_RAM_EXEC)
enable_language(ASM)
enable_language(C)
enable_language(CXX)

# 输出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# comfirm minimum version cmake
cmake_minimum_required(VERSION 3.0.0)
project(startup)

# source files
file(GLOB_RECURSE SRC_C_FILES ${ROOT}/Application/*.c ${ROOT}/CMSIS/*.c ${ROOT}/Drivers/*.c ${ROOT}/Middlewares/*.c)
file(GLOB_RECURSE SRC_CXX_FILES ${ROOT}/Application/*.cpp ${ROOT}/CMSIS/*.cpp ${ROOT}/Drivers/*.cpp ${ROOT}/Middlewares/*.cpp)

include_directories(${ROOT}/CMSIS/Compiler ${ROOT}/CMSIS/Core ${ROOT}/CMSIS/Device/Include ${ROOT}/Application ${ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc ${ROOT}/Drivers/STM32H7xx_HAL_Driver ${ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy ${ROOT}/Drivers/User/Inc ${ROOT}/Scripts)

set_source_files_properties(${ROOT}/startup_self_define.S PROPERTIES LANGUAGE ASM)
set_source_files_properties(${ROOT}/bootloader.S PROPERTIES LANGUAGE ASM)

# project settings
set(EXECUTABLE_OUTPUT_PATH ${ROOT}/build/output)
set(CMAKE_BINARY_DIR ${ROOT}/build)
add_executable(startup.elf ${ROOT}/startup_self_define.S ${ROOT}/bootloader.S ${SRC_C_FILES} ${SRC_CXX_FILES})

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_BUILD_TYPE Debug)


# 添加以下代码，确保CompilerID目标与主项目分离
set_target_properties(startup.elf PROPERTIES
    COMPILE_DEFINITIONS "CMAKE_SKIP_COMPILER_ID_TESTS=1"
)
