# 环境搭建
开发系统：WSL下的Ubuntu22.04-LTS，配合zsh终端
工具链选择：gcc-arm-none-eabi (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119
cmake：cmake version 3.22.1
make：GNU Make 4.3 Built for x86_64-pc-linux-gnu
调试工具：STLINK-V2
STLINK驱动程序：v1.7.0
openOCD：Open On-Chip Debugger 0.11.0
git：git version 2.34.1

**请特别注意添加工具链路径到PATH中**


# target
使用的是STM32H743XIH6(反客科技)
目标平台：none
处理器：ARM Cortex-M7 with FPU without MMU

# 部署实验
1. 将workspace解压至或者git clone至/根目录下
2. 命令行运行./build.sh
3. 在build/output中找到startup.elf
4. 利用openOCD和arm-none-eabi-gdb开启连接并调试

# 与LED实验不同的地方
1. 此实验开发源码由LED源码提供
2. 此工程的创建目的在于实现.isr_vector .text .rodata放置于SRAM，并成功启动运行
3. 修改了startup.S文件(考虑到工程纯净性，已经将ST提供的startup.S文件丢弃，如需要对照，请参照LED根目录下的startup_stm32h743xx.S)
4. 创建了bootloader.S，唯一的功能是实现代码的复制
5. 修改了stm32h743xi_flash.ld，注意本工程并未使用stm32h743xi_ram.ld，对生成的ELF文件的SECTION进行了规划
6. BOOT引脚仍为0，且并未改变BOOT_ADD0，仍然在0x08000000

# 仍待解决的问题和策略
1. 还并未验证中断的触发能否找到中断向量表（理论上来说可以） **√**
  - 这里加了一个每1s手动触发TIM3更新中断的程序，也就是这个工程最终的代码形式，处于他能涵盖本工程的本来目的，我就没有再改了，或者保留上一版，考虑到本工程的文件结构非常简单，所以读者可以根据自己的需求做出一些修改来验证自己的想法。
2. 复位后仍然会进行一次代码的copy，对于调试极为不利，所以希望寻找一个非易失性内存区域做一个tag以方便复位直接运行程序（初步设想是直接判断RAM中isr_vector所在的地址是否由内容，无内容则需要进行复制） **√**
  - 采用括号内的想法，效果还不错，就是更新固件的时候不太方便，调试起来还行。


# Note
1. 现在只是一个建议版本，后续我会将在VSCODE上的调试版本进行更新，敬请期待...

2. 在Docs中我准备了一些文档以供大家查阅相关的知识
