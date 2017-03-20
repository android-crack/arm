    GET		s3c2410_SFR.s	;GET伪指令将s3c2410_SFR.s包含到此文件中,s3c2410_SFR.s是寄存器地址的宏定义
	GET		startup_head.s	;GET伪指令将startup_head.s包含到此文件中,startup_head.s是初始化配置

	IMPORT  main			;IMPORT伪指令指示编译器当前的符号不是在本源文件中定义的,而是在其它源文件中定义的,在本源文件中可能引用该符号,main定义在c源文件中
	IMPORT	Handle_UART0 	;Handle_UART0定义在c源文件中
	
	AREA   	RESET, CODE, READONLY 	;定义一个名为RESET的只读代码段
	CODE32							;CODE32伪指令指示汇编编译器后面的指令为32位的ARM指令
	ENTRY							;ENTRY伪指令用于指定程序的入口点,一个程序(可以包含多个源文件)中至少要有一个ENTRY,可以有多个ENTRY，但一个源文件中最多只有一个ENTRY

_Startup								;这只是一个普通标号
	EXPORT	VectorBase					;EXPORT声明一个符号VectorBase可以被其它文件引用

VectorBase								;向量基址,下面是自定义的向量集
	B		HandlerReset				;直接跳转到HandlerReset处进行处理
	LDR		PC, (Vect_Table + 4)		;将(Vect_Table + 4)中的地址加载到PC中,也就是跳转到(Vect_Table + 4)中的地址处进行处理
	LDR		PC, (Vect_Table + 8)		;SWI,将(Vect_Table + 8)中的地址加载到PC中,也就是跳转到(Vect_Table + 8)中的地址处进行处理
	LDR		PC, (Vect_Table +12)		;Prefetch Abort,将(Vect_Table + 12)中的地址加载到PC中,也就是跳转到(Vect_Table + 12)中的地址处进行处理
	LDR		PC, (Vect_Table +16)		;Data Abort,将(Vect_Table + 16)中的地址加载到PC中,也就是跳转到(Vect_Table + 16)中的地址处进行处理
	B		.							;Not Assigned,什么事也不做,相当于while(1)
	LDR		PC, (Vect_Table +24)		;IRQ,将(Vect_Table + 24)中的地址加载到PC中,也就是跳转到(Vect_Table + 24)中的地址处进行处理 
	LDR		PC, (Vect_Table +28)		;FIQ,将(Vect_Table + 28)中的地址加载到PC中,也就是跳转到(Vect_Table + 28)中的地址处进行处理 

Vect_Table								;中断向量表,DCD用于分配一片连续的字(4个字节)存储单元并用指定的数据初始化(有点像int型数组)
	DCD		HandlerReset				;相当于(Vect_Table + 0),并且将HandlerReset的值加载到其中
	DCD		HandlerUndef				;相当于(Vect_Table + 4),并且将HandlerUndef的值加载到其中
	DCD		HandlerSWI					;相当于(Vect_Table + 8),并且将HandlerSWI的值加载到其中
	DCD		HandlerPabort				;相当于(Vect_Table + 12),并且将HandlerPabort的值加载到其中
	DCD		HandlerDabort				;相当于(Vect_Table + 16),并且将HandlerDabort的值加载到其中
	DCD		.							;相当于(Vect_Table + 20),并且将.的值加载到其中  
	DCD	    IRQ_Handler					;相当于(Vect_Table + 24),并且将IRQ_Handler的值加载到其中
	DCD		HandlerFIQ					;相当于(Vect_Table + 28),并且将HandlerFIQ的值加载到其中
	DCD		main						;相当于(Vect_Table + 32),并且将main的值加载到其中
		
	EXPORT VectorEnd					;EXPORT声明一个符号VectorEnd可以被其它文件引用
VectorEnd	
	LTORG								;声明文字池保存以上向量表(这条命令的实际效用还是有点不是很清楚)

	AREA   	RESET, CODE, READONLY		;定义一个名为RESET的只读代码段
HandlerReset							;定义一个HandlerReset标签(指代了此处的地址)
;/***************************************/
;/* disable interrupt                   */
;/***************************************/
	MRS 	R0, cpsr    				;将状态寄存器cpsr中的值读到R0中
    ORR 	R0, R0, #0xc0				;将R0与(1100 0000)进行或操作,结果放到R0中,这个过程其实是保持其它位不变，将第6(FIQ)位和7(IRQ)位置1，就是禁止所有中断
    MSR 	cpsr_c, R0					;将R0重新存回,也就是关闭了所有中断

;/***************************************/
;/* disable watchdog                    */
;/***************************************/
	LDR		R0, =WTCON					;看门狗配置寄存器地址加载到R0中
	LDR		R1, =0x0         			;将0加载到R1中
	STR		R1, [R0]					;将看门狗配置寄存器中的值置0,也就是关闭看门狗

;/****************************************/
;/* config interrupt                     */
;/****************************************/
	LDR		R0, =INTMSK					;中断配置寄存器地址加载到R0中
	LDR		R1, =0xFFFFFFFF             ;将全1加载到R1中
	STR		R1, [R0]					;将全1加载到中断配置寄存器中，让所有中断屏蔽掉

	LDR		R0, =INTSUBMSK				;子中断配置寄存器地址加载到R0中
	LDR		R1, =0x000007FF		        ;将(0111 1111 1111)加载到R1中
	STR		R1, [R0]					;将全(0111 1111 1111)加载到子中断配置寄存器中，让所有子中断屏蔽掉

	LDR     R0, =INTPND                 ;中断未决寄存器地址加载到R0中
	LDR     R1, =0xFFFFFFFF				;将全1加载到R1中
	STR     R1, [R0]					;通过写1的方式来清理中断未决寄存器
	
	LDR     R0, =SRCPND                 ;源未决寄存器地址加载到R0中
	LDR     R1, =0xFFFFFFFF				;将全1加载到R1中
	STR     R1, [R0]					;通过写1的方式来清理源未决寄存器

;/****************************************/
;/* config pll                           */
;/****************************************/
	LDR		R0, =LOCKTIME				;锁定时间计数寄存器地址加载到R0中
	LDR		R1, =0x00FFFFFF				;将0x00FFFFFF加载到R1中
	STR		R1, [R0]					;将R1加载到锁定时间计数寄存器中(U_LTIME 为0x00FF,M_LTIME 为0xFFFF)

	LDR		R0, =CLKDIVN		        ;时钟分频控制寄存器地址加载到R0中
	LDR		R1, =((M_HDIVN << 1) | M_PDIVN)	;将((M_HDIVN << 1) | M_PDIVN)加载到R1中，M_HDIVN 和 M_PDIVN 定义在另一个头文件中
	STR		R1, [R0]					;进行配置

;/****************************************/
;/* config mmu                           */
;/****************************************/
	MRC  	p15, 0, R0, c1, c0, 0
	ORR		R0, R0, #R1_nF:OR:R1_iA
	MCR		p15,0,R0,c1,c0,0
	
	MRC		p15, 0, R0, c1, c0, 0
	BIC		R0, R0, #R1_M
	MCR		p15, 0, R0, c1, c0, 0
	
	MRC		p15, 0, R0, c1, c0, 0
	ORR		R0, R0, #R1_I
	MCR		p15, 0, R0, c1, c0, 0
	
	MRC		p15, 0, R0, c1, c0, 0
	ORR		R0, R0, #R1_C
	MCR		p15, 0, R0, c1, c0, 0

;/****************************************/
;/* config pll                           */
;/****************************************/
	LDR		R0, =CLKCON			        ;时钟发生器控制寄存器地址加载到R0中
	LDR		R1, =0x0007FFF0				;将0x0007FFF0加载到R1中，相应的位置1，就是设定有效，哪些外设要进行有效处理，得查文档
	STR		R1, [R0]					;进行设定

	LDR		R0, =CLKSLOW				;减慢时钟控制寄存器地址加载到R0中
	LDR		R1, =0x00000004				;将0x00000004加载到R1中
	STR		R1, [R0]					;进行设定

	LDR     R0, =UPLLCON  				;UPLL配置寄存器地址加载到R0中 ,USB的PLL就在此配置
	LDR     R1, =((U_MDIV << 12) + (U_PDIV << 4) + U_SDIV) ;将((U_MDIV << 12) + (U_PDIV << 4) + U_SDIV)加载到R1中 ;Fin=12MHz, Fout=48MHz
	STR     R1, [R0]					;进行设定

	NOP									;NOP为空操作伪指令，NOP伪指令在汇编时将会被代替成ARM中的空操作，比如 MOV R0,R0
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

	LDR		R0, =MPLLCON                ;MPLL配置寄存器地址加载到R0中
	LDR		R1, =((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV) ;将((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV)加载到R1中
	STR		R1, [R0]					;进行设定

	NOP
	NOP

;/****************************************/
;/* config stack                         */
;/****************************************/
    MSR     CPSR_c, #0x0d2				;将(1101 0010)加载到CPSR_c中(代表禁止所有中断,使用ARM模式,进入中断模式)
    LDR     SP, =IRQStack_BASE			;IRQStack_BASE在另一个文件中定义,将中断模式中的堆栈指针SP指到IRQStack_BASE处

    MSR     CPSR_c, #0x05f				;将(0101 1111)加载到CPSR_c中(代表开启IRQ中断禁止FIQ中断,使用ARM模式,进入系统模式)
    LDR     SP, =UsrStack_BASE			;UsrStack_BASE在另一个文件中定义,将系统模式中的堆栈指针SP指到UsrStack_BASE处

;/***************************************/
;/* enable interrupt                    */
;/***************************************/
	MRS 	R0, cpsr    				;cpsr加载到R0中
    BIC 	R0, R0, #0x80				;BIC将R0中的第7位置0,(将R0跟(1000 0000)的反码(0111 1111)进行与操作,就是对第7位清零)
    MSR 	cpsr_c, R0					;将R0结果保存回cpsr_c中

;/****************************************/
;/* go to c main                         */
;/****************************************/
	LDR		PC, (Vect_Table + 32)		; 这里进行跳转，相当于 goto main(由此可知c语言中的main函数之所以叫main,也是类似这样的地方定义的,如果取别的名字比如xxx,那C的代码就都会从xxx函数开始执行)
	NOP
	NOP
	NOP
	NOP

;/****************************************/
;/* Undefined Instruction interrupt entry*/
;/****************************************/
HandlerUndef							;定义一个HandlerUndef,并且啥也不干
	B		.

;/****************************************/
;/* SWI interrupt entry                  */
;/****************************************/
HandlerSWI								;定义一个HandlerSWI,并且啥也不干
	B		.

;/****************************************/
;/* Prefetch Abort interrupt entry       */
;/****************************************/
HandlerPabort							;定义一个HandlerPabort,并且啥也不干
	B		.

;/****************************************/
;/* Data Abort interrupt entry           */
;/****************************************/
HandlerDabort							;定义一个HandlerDabort,并且啥也不干
	B		.
	
;/****************************************/
;/* FIQ interrupt entry                  */
;/****************************************/
HandlerFIQ								;定义一个HandlerFIQ,并且啥也不干
	B		.

;/****************************************/
;/* default irq entry                    */
;/****************************************/
	EXPORT  Default_IRQ_ISR				;EXPORT声明一个符号Default_IRQ_ISR可以被其它文件引用
Default_IRQ_ISR							;定义一个Default_IRQ_ISR,并且啥也不干
	B       .

	PRESERVE8 							;保证堆栈8字节对齐

IRQ_Vecotr                            	;中断向量表
EINT0_Handle       B   Default_IRQ_ISR 	;B   Default_IRQ_ISR代表啥都不干(因为上面对Default_IRQ_ISR中的操作定义就是啥都没干)
EINT1_Handle       B   Default_IRQ_ISR
EINT2_Handle       B   Default_IRQ_ISR
EINT3_Handle       B   Default_IRQ_ISR
EINT4_7_Handle     B   Default_IRQ_ISR
EINT8_23_Handle    B   Default_IRQ_ISR
CAM_Handle         B   Default_IRQ_ISR
BATFLT_Handle      B   Default_IRQ_ISR
TICK_Handle        B   Default_IRQ_ISR
WDT_AC97_Handle    B   Default_IRQ_ISR
ISR_TIMER0_Handle  B   Default_IRQ_ISR
ISR_TIMER1_Handle  B   Default_IRQ_ISR
ISR_TIMER2_Handle  B   Default_IRQ_ISR
ISR_TIMER3_Handle  B   Default_IRQ_ISR
ISR_TIMER4_Handle  B   Default_IRQ_ISR
ISR_UART2_Handle   B   Default_IRQ_ISR
ISR_LCD_Handle     B   Default_IRQ_ISR
ISR_DMA0_Handle    B   Default_IRQ_ISR
ISR_DMA1_Handle    B   Default_IRQ_ISR
ISR_DMA2_Handle    B   Default_IRQ_ISR
ISR_DMA3_Handle    B   Default_IRQ_ISR
ISR_SDI_Handle     B   Default_IRQ_ISR
ISR_SPI0_Handle    B   Default_IRQ_ISR
ISR_UART1_Handle   B   Default_IRQ_ISR
ISR_NFCON_Handle   B   Default_IRQ_ISR
ISR_USBD_Handle    B   Default_IRQ_ISR
ISR_USBH_Handle    B   Default_IRQ_ISR
ISR_IIC_Handle     B   Default_IRQ_ISR
ISR_UART0_Handle   B   Handle_UART0		;收到ISR_UART0_Handle中断会跳转到Handle_UART0进行处理
ISR_SPI1_Handle    B   Default_IRQ_ISR
ISR_RTC_Handle     B   Default_IRQ_ISR
ISR_ADC_Handle     B   Default_IRQ_ISR

IRQ_Handler     PROC
	EXPORT  IRQ_Handler               [WEAK] ;EXPORT声明一个符号IRQ_Handler可以被其它文件引用，[WEAK] 指定该选项后，如果symbol在所有的源程序中都没有被定义，编译器也不会产生任何错误信息，同时编译器也不会到当前没有被INCLUDE进来的库中去查找该标号

    SUB		LR, LR, #4						;LR连接寄存器(Link Register, LR)，在ARM体系结构中LR的特殊用途有两种：一是用来保存子程序返回地址；二是当异常发生时，LR中保存的值等于异常发生时PC的值减4(或者减2),因此在各种异常模式下可以根据LR的值返回到异常发生前的相应位置继续执行
    STMDB	SP!, {R0-R12,LR}				;保护现场,将{R0-R12,LR}作压栈处理,顺序是寄存器从大到小，SP!意思是每次操作完将SP更新的值还是存回SP
    LDR		R0, =INTOFFSET					;将中断偏移寄存器的地址存到R0中
    LDR		R0, [R0]						;将R0中地址(中断偏移寄存器地址)所代表的寄存器的值存到R0中
    LDR		R1, =IRQ_Vecotr					;将中断向量表的基址存到R1中
    ADD		R1, R1, R0, LSL #2				;将R0逻辑左移2位,加上R1,结果放到R1中,其实就是R1=R1+R0*4,为什么要乘4呢,因为向量表是4字节对其的,所以结果就是相应中断跳转的位置
    LDR		LR, =int_return					;LR中保存int_return作为返回地址
    MOV		PC, R1							;将R1的值(中断入口地址)保存到PC中,即相当于直接跳转到中断处,开始执行中断服务程序

int_return									;返回地址
    LDMIA SP!,{R0-R12, PC}^					;进行现场恢复,将之前压栈的环境变量从堆栈中读出,覆盖到当前的寄存器中,在LDM指令的寄存器列表中包含有PC时使用'^',那么除了正常的多寄存器传送外,将SPSR拷贝到CPSR中,这可用于异常处理返回,使用'^'后缀进行数据传送且寄存器列表不包含PC时,加载/存储的是用户模式的寄存器,而不是当前模式的寄存器
	
    ENDP

	END

