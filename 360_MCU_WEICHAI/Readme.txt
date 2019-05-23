芯    片：STM32F105VC(TQFP100), Cortex-M3, 256KB Flash, 64KB RAM, 72MHz
开发工具：
1）支持商业软件MDK-ARM，Keil uVision，目前软件版本v4.74
2）支持开源软件EmBlocks，基于GNU GCC，目前软件版本v2.3
芯    片：STM32F105VC(TQFP100), Cortex-M3, 256KB Flash, 64KB RAM, 72MHz
开发工具：
1）支持商业软件MDK-ARM，Keil uVision，目前软件版本v4.74
2）支持开源软件EmBlocks，基于GNU GCC，目前软件版本v2.3

文件目录说明：
1)
根目录下文件说明
Readme.txt: 本文档

2)
MCU的Flash分成3个区：
0x08000000 - 0x080047FF			Boot分区, 18KB
0x08004800 - 0x08004FFF         Flag分区, 2KB，Page size = 2KB
0x08005000 - 0x0803FFFF			App分区，236KB
MCU的SRAM分成2个区：
0x20000000 - 0x200003FF			1KB的NoInit RAM区，用做Share mem
0x20000400 - 0x2000FFFF			63KB用作普通用途，stack, global, heap...
Keil是在工程配置界面设置的
Emblocks是在另一个文件xxx.ld里面设置的

3)Boot目录存放的是Boot功能相关代码以及头文件(含软件配置头文件config.h)

4)App目录存放的是App功能相关代码以及头文件(含软件配置头文件config.h)

5)Lib目录存放的是实现Boot和App功能需要的底层支持API，全部使用寄存器读写实现，还包括汇编代码实现的API以及启动代码向量表等。

ST_SDK目录存放的是ST提供的库源码，考虑到代码size和效率问题，所以没有使用ST提供的库，但是用到了里面的芯片相关以及外设寄存器定义头文件，要正确引用里面的头文件，需要在工程文件Option->C/C++里面定义宏:
STM32F10X_CL
并添加头文件相关路径
ST_SDK里面相关头文件已经移动到 Inc\core 目录

6)Inc目录存放的是Boot和App公用的头文件以(含硬件配置头文件hw_config.h)

7)Project目录存放的是Keil/EmBlocks工程文件，包括Boot和App的，编译生成的所有文件也位于该目录
mergebin.bat: 按照Flash存储布局，将Boot.bin和App.bin合并生成Flash镜像flash.bin，用于初次Flash烧写

8)Bin目录用于存放7)中所述二进制.bin文件
