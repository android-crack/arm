;input frequency	12.00 MHz
;MPLL的分频配置 		;MPLL=(2*m*Fin)/(p*2^s)
M_MDIV		EQU		127	;m=(MDIV+8)
M_PDIV		EQU		2	;p=(PDIV+2)		
M_SDIV		EQU		1	;s=SDIV
; output frequency		405.00 MHz

; hdivn,pdivn FCLK:HCLK:PCLK
;     0,0         1:1:1 
;     0,1         1:1:2 
;     1,0         1:2:2
;     1,1         1:2:4
M_HDIVN		EQU		1	;HDIVN=01  代表HCLK=FCLK/2
M_PDIVN		EQU		1	;PDIVN=01  代表PCLK=HCLK/2
;所以FCLK:HCLK:PCLK=4:2:1


;Fin=12.0MHz 
;UPLL的分频配置 ; UPLL=(m*Fin)/(p*2^s)
U_MDIV		EQU		56	;m=(MDIV+8)	
U_PDIV		EQU		2	;p=(PDIV+2)
U_SDIV		EQU		2	;s=SDIV
;Fout=48.0MHz

R1_I    EQU     (1<<12)			;Rx_y 这一系列是在定义变量，分别代表R1寄存器上的不同位
R1_C    EQU     (1<<2)
R1_A    EQU     (1<<1)
R1_M    EQU     (1)
R1_iA   EQU     (1<<31)
R1_nF   EQU     (1<<30)


STACK_SIZE	EQU	128		;定义变量栈大小	
SUB_STACK_SIZE	EQU 	128		;定义变量子栈大小
	
STACK_BASE	EQU	(0x00001000)	;定义栈的基址
IRQStack_BASE	EQU	STACK_BASE			;定义IRQ栈的基址
UsrStack_BASE	EQU	(STACK_BASE - SUB_STACK_SIZE)	;定义用户栈的基址


	END