о    Ƭ��STM32F105VC(TQFP100), Cortex-M3, 256KB Flash, 64KB RAM, 72MHz
�������ߣ�
1��֧����ҵ���MDK-ARM��Keil uVision��Ŀǰ����汾v4.74
2��֧�ֿ�Դ���EmBlocks������GNU GCC��Ŀǰ����汾v2.3
о    Ƭ��STM32F105VC(TQFP100), Cortex-M3, 256KB Flash, 64KB RAM, 72MHz
�������ߣ�
1��֧����ҵ���MDK-ARM��Keil uVision��Ŀǰ����汾v4.74
2��֧�ֿ�Դ���EmBlocks������GNU GCC��Ŀǰ����汾v2.3

�ļ�Ŀ¼˵����
1)
��Ŀ¼���ļ�˵��
Readme.txt: ���ĵ�

2)
MCU��Flash�ֳ�3������
0x08000000 - 0x080047FF			Boot����, 18KB
0x08004800 - 0x08004FFF         Flag����, 2KB��Page size = 2KB
0x08005000 - 0x0803FFFF			App������236KB
MCU��SRAM�ֳ�2������
0x20000000 - 0x200003FF			1KB��NoInit RAM��������Share mem
0x20000400 - 0x2000FFFF			63KB������ͨ��;��stack, global, heap...
Keil���ڹ������ý������õ�
Emblocks������һ���ļ�xxx.ld�������õ�

3)BootĿ¼��ŵ���Boot������ش����Լ�ͷ�ļ�(���������ͷ�ļ�config.h)

4)AppĿ¼��ŵ���App������ش����Լ�ͷ�ļ�(���������ͷ�ļ�config.h)

5)LibĿ¼��ŵ���ʵ��Boot��App������Ҫ�ĵײ�֧��API��ȫ��ʹ�üĴ�����дʵ�֣�������������ʵ�ֵ�API�Լ���������������ȡ�

ST_SDKĿ¼��ŵ���ST�ṩ�Ŀ�Դ�룬���ǵ�����size��Ч�����⣬����û��ʹ��ST�ṩ�Ŀ⣬�����õ��������оƬ����Լ�����Ĵ�������ͷ�ļ���Ҫ��ȷ���������ͷ�ļ�����Ҫ�ڹ����ļ�Option->C/C++���涨���:
STM32F10X_CL
�����ͷ�ļ����·��
ST_SDK�������ͷ�ļ��Ѿ��ƶ��� Inc\core Ŀ¼

6)IncĿ¼��ŵ���Boot��App���õ�ͷ�ļ���(��Ӳ������ͷ�ļ�hw_config.h)

7)ProjectĿ¼��ŵ���Keil/EmBlocks�����ļ�������Boot��App�ģ��������ɵ������ļ�Ҳλ�ڸ�Ŀ¼
mergebin.bat: ����Flash�洢���֣���Boot.bin��App.bin�ϲ�����Flash����flash.bin�����ڳ���Flash��д

8)BinĿ¼���ڴ��7)������������.bin�ļ�
