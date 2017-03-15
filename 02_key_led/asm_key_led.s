;﻿;;
;LED电路原理图
;LED1-GPB5
;LED2-GPB6
;LED3-GPB7
;LED4-GPB8
;0-On,1-Off
;芯片针脚配置寄存器
;GPBCON-0x56000010
;GPBDAT-0x56000014
;GPBUP -0x56000018
;KEY电路原理图
;K1-EINT1/GPF1
;K2-EINT4/GPF4
;K3-EINT2/GPF2
;K4-EINT0/GPF0
;DOWN-0,UP-1
;芯片针脚配置寄存器
;GPFCON-0x56000050
;GPFDAT-0x56000054
;GPFUP -0x56000058

	AREA    RESET, CODE, READONLY ;定义一个名为RESET的只读代码段
	CODE32 ;CODE32伪指令指示汇编编译器后面的指令为32位的ARM指令
	ENTRY ;ENTRY伪指令用于指定程序的入口点,一个程序(可以包含多个源文件)中至少要有一个ENTRY,可以有多个ENTRY，但一个源文件中最多只有一个ENTRY

START ;不是关键词,只是一个标号,可以修改
;关看门狗相关配置
	LDR		R0, =0x53000000 ;0x53000000是看门狗定时器的控制寄存器地址,LDR是将这个地址加载到R0寄存器中
	LDR		R1, =0         	;LDR将0这个立即数，加载到R1寄存器中
	STR		R1, [R0]		;STR将R1寄存器中的值(0)存到R0寄存器中地址所指代的寄存器中,在这里，总体来讲就是关闭了看门狗

;配置与按键相连接的IO端口为输入GPF{1,4,2,0},00代表输入
	LDR		R0, =0x56000050 ;0x56000050是GPF的控制寄存器地址,LDR是将这个地址加载到R0寄存器中
	LDR		R1, =0        	;LDR将0这个立即数，加载到R1寄存器中
	STR		R1, [R0]		;STR将R1寄存器中的值(0)存到R0寄存器中地址所指代的寄存器中,在这里，总体来讲就是将所有的GPF针脚都设定成了输入模式，00代表输入

;配置与LED相连接的IO端口为输出GPB{5,6,7,8},01代表输出
	LDR		R2, =0x56000010	;0x56000010是GPB的控制寄存器地址,LDR是将这个地址加载到R2寄存器中
	LDR		R3, =0x00015400 ;LDR将0x00015400这个数,加载到R3寄存器中,0x00015400代表0001 0101 0100 0000 0000,[17-10]分别为01010101,代表GPB{5,6,7,8}设定为输出,01代表输出
	STR		R3, [R2]	;STR将R3寄存器中的值(0x00015400)存到R2寄存器中地址所指代的寄存器中,在这里,总体来讲就是将GPB{5,6,7,8}设定为输出,01代表输出

;熄灭全部LED灯
	LDR		R2, =0x56000014	;0x56000014是GPB的数据寄存器地址,LDR是将这个地址加载到R2寄存器中
	LDR		R3, =0x000001E0 ;LDR将0x000001E0这个数加载到R3寄存器中,0x000001E0代表0001  1110 0000,代表GPB{5,6,7,8}的位置为高电位 
	STR		R3, [R2]		;STR将R3寄存器中的值(0x000001E0)存到R2寄存器中地址所指代的寄存器中,在这里,总体来讲就是将GPB{5,6,7,8}设定为高电位,高电位LED灯就灭了

LOOP
;读取按键状态
	LDR		R0, =0x56000054		;0x56000054是GPFDAT寄存器的地址,LDR将0x56000054这个地址加载到R0寄存器中
	LDR		R1, [R0]			;LDR将R0寄存器中地址所指代的寄存器中的值加载到R1
;LED初始状态为熄灭				
	LDR		R3, =0x000001E0		;LDR将0x000001E0(0001 1110 0000)加载到R3中
;判断K1是否按下
	TST		R1, #0x00000002		;TST指令将寄存器R1的值与0x00000002(0000 0010)的值按位作逻辑“与”操作，根据操作的结果更新CPSR中的相应条件标志位,以便后面的指令根据相应的条件标志来判断是否执行,此操作类似于 if((GPFDAT & 0x00000002) == 0){},TST指令与ANDS指令的区别在于TST指令不保存运算结果,TST指令通常与EQ/NE条件码配合使用,当所有测试位均为0时,EQ有效,而只要有一个测试位不为0,则NE有效
	BICEQ	R3, #0x00000020		;如果上一步的结果为EQ,则此步的操作就是将R3寄存器中值的第5位(从0位开始数)值清零,BIC指令将寄存器R3的值与0x00000020(0010 0000)的反码(1101 1111)作逻辑“与”操作，结果保存到R3寄存器中;GPB5 
;判断K2是否按下
	TST		R1, #0x00000010		;TST指令将寄存器R1的值与0x00000010(0001 0000)的值按位作逻辑“与”操作，根据操作的结果更新CPSR中的相应条件标志位,以便后面的指令根据相应的条件标志来判断是否执行,此操作类似于 if((GPFDAT & 0x00000010) == 0){},TST指令与ANDS指令的区别在于TST指令不保存运算结果,TST指令通常与EQ/NE条件码配合使用,当所有测试位均为0时,EQ有效,而只要有一个测试位不为0,则NE有效
	BICEQ	R3, #0x00000040		;如果上一步的结果为EQ,则此步的操作就是将R3寄存器中值的第6位(从0位开始数)值清零,BIC指令将寄存器R3的值与0x00000040(0100 0000)的反码(1011 1111)作逻辑“与”操作，结果保存到R3寄存器中;GPB6
;判断K3是否按下
	TST		R1, #0x00000004		;TST指令将寄存器R1的值与0x00000004(0000 0100)的值按位作逻辑“与”操作,根据操作的结果更新CPSR中的相应条件标志位,以便后面的指令根据相应的条件标志来判断是否执行,此操作类似于 if((GPFDAT & 0x00000004) == 0){},TST指令与ANDS指令的区别在于TST指令不保存运算结果,TST指令通常与EQ/NE条件码配合使用,当所有测试位均为0时,EQ有效,而只要有一个测试位不为0,则NE有效
	BICEQ	R3, #0x00000080		;如果上一步的结果为EQ,则此步的操作就是将R3寄存器中值的第7位(从0位开始数)值清零,BIC指令将寄存器R3的值与0x00000080(1000 0000)的反码(0111 1111)作逻辑“与”操作，结果保存到R3寄存器中;GPB7
;判断K4是否按下
	TST		R1, #0x00000001		;TST指令将寄存器R1的值与0x00000001(0000 0001)的值按位作逻辑“与”操作,根据操作的结果更新CPSR中的相应条件标志位,以便后面的指令根据相应的条件标志来判断是否执行,此操作类似于 if((GPFDAT & 0x00000001) == 0){},TST指令与ANDS指令的区别在于TST指令不保存运算结果,TST指令通常与EQ/NE条件码配合使用,当所有测试位均为0时,EQ有效,而只要有一个测试位不为0,则NE有效
	BICEQ	R3, #0x00000100		;如果上一步的结果为EQ,则此步的操作就是将R3寄存器中值的第8位(从0位开始数)值清零,BIC指令将寄存器R3的值与0x00000100(0001 0000 0000)的反码(1110 1111 1111)作逻辑“与”操作，结果保存到R3寄存器中;GPB8
;控制LED显示
	STR		R3, [R2] 			;STR将R3寄存器中的值存到R2寄存器中地址所指代的寄存器中,在这里,总体来讲就是将GPB{5,6,7,8}设定为相应电位,高电位LED灯就灭了,低电位灯就亮了
	B		LOOP 				;不断循环

	END						;汇编结束