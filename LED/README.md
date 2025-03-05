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

# Note
1. 现在只是一个建议版本，后续我会将在VSCODE上的调试版本进行更新，敬请期待...

2. 在Docs中我准备了一些文档以供大家查阅相关的知识