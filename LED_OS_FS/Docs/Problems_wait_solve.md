1. 关于启动流程之硬件部分的梳理--Reset或者上电 **√**
详见StartupWithoutMMU.md
3. .glue_7和.glue_7t两者的来源 **√**
详见loadDomain_and_exeDomain.md
4. .eh_frame这一段的来源
5. .rodata的存放位置 **√**
暂无参考，可自行实验验证
6. 中断处理程序能否直接借助它的.ld配置直接放到RAM中去 **√**
不能，需要自行编写loader复制代码，详见stm32h7x_self_define.ld
7. 自定义需要放到RAM的代码段
8. 为什么.user_heap_stack的当前定义位置是程序映像的结束地址
9. 解析RAM软件启动流程 **√**