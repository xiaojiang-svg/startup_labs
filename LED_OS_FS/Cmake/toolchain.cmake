set(bin_path /opt/gcc-arm-none-eabi/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin)

# set target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR armv7)

# 禁用编译器启动自动测试
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 指定C库配置（使用nano库减小体积，并忽略系统调用依赖）
set(CMAKE_C_FLAGS_INIT "--specs=nano.specs --specs=nosys.specs")
set(CMAKE_CXX_FLAGS_INIT "--specs=nano.specs --specs=nosys.specs")
set(CMAKE_ASM_FLAGS_INIT "--specs=nosys.specs")

# 全局编译选项
set(CMAKE_C_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -ffreestanding -fno-builtin -DSTM32H743xx -DUSE_HAL_DRIVER -O0 " CACHE STRING "C Flags")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "C++ Flags")

# 添加汇编预处理支持
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp -mcpu=cortex-m7 -mthumb" CACHE STRING "ASM Flags")

# confirm toolchain we use
set(CMAKE_C_COMPILER ${bin_path}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${bin_path}/arm-none-eabi-g++)

set(CMAKE_ASM_COMPILER ${bin_path}/arm-none-eabi-gcc)
set(CMAKE_LINKER ${bin_path}/arm-none-eabi-ld)
set(CMAKE_AR ${bin_path}/arm-none-eabi-ar)
set(CMAKE_NM ${bin_path}/arm-none-eabi-nm)
set(CMAKE_OBJCOPY ${bin_path}/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP ${bin_path}/arm-none-eabi-objdump)
set(CMAKE_STRIP ${bin_path}/arm-none-eabi-strip)
set(CMAKE_READELF ${bin_path}/arm-none-eabi-readelf)

# 链接器配置
set(LINKER_SCRIPT /workspace/LED_OS_FS/Scripts/stm32h743xi_flash.ld)
set(CMAKE_EXE_LINKER_FLAGS
    "-T${LINKER_SCRIPT} -Wl,--gc-sections"
    CACHE STRING "Linker Flags"
)


# set compiling conditions
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)