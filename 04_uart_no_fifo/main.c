#include "2440addr.h"			//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏
	
//UART的 no fifo 模式,从串口获取数据,根据输入值控制灯与蜂鸣器

unsigned char FlagRec = 0x00;	//定义一个标志,进行中断状态的记录与区别
unsigned char RecLen,RecData2;	//定义一个缓存区,和一个长度记录的变量

void ledbeepinit()				//初始化beep和led
{
	rGPBCON = (1<<0)|(1<<10) |(1<<12)|(1<<14)|(1<<16); 	//设定 GPB{0,5,6,7,8} 为输出
	rGPBDAT =  (1<<5)| (1<<6)|(1<<7)|(1<<8);			//将所有的LED熄灭,将蜂鸣器关闭
}

void UART0Init(void)			//初始化UART0
{
	rGPHCON = (2<<6) | (2<<4);	//配置为串口收发RXD[0],TXD[0]
	rGPHUP  = (1<<3) | (1<<2);	//禁用上拉电阻
	
	rUBRDIV0= 658;				//波特率除数寄存器,UBRDIVn=(int)(UART_clock/(buad_rate*16))-1, 这里波特率	9600  PCLK=101.25M ;   101.25M/(9600*16) - 1 = 658
	
	rULCON0 = (0<<6) | (0<<3) | (0<<2) | (3<<0);	//发送接收每帧的数据位数为8bit,每帧一个停止位,没有奇偶校验，正常模式操作(非红外模式),将0号通道的线性控制寄存器设定为(0000 0011)
	rUCON0  = (1<<2) | (1<<0); 	//(0000 0101),UART控制寄存器,设定为收发中断模式
	rUFCON0 = (0<<6) | (1<<4) | (1<<2) | (1<<1) | (0<<0);	//(0001 0110)发送fifo触发为空,读取fifo触发为8字节,发送fifo重置后自动清除,读取fifo重置后自动清除,禁用fifo特性
	rUMCON0 = (0<<4);		   	//串行Modem控制寄存器,关闭AFC(Auto Flow Control),使用高电平去激活nRTS('H' level)

	
	EnableSubIrq(BIT_SUB_RXD0);	//开启串口0接收子中断
	EnableIrq(BIT_UART0);		//开启串口0中断
}


void Handle_UART0(void)			//串口中断服务程序
{
	if(rUTRSTAT0 & 1)			//判断接收缓冲是否有数据(第[0]位代表 Receive buffer data ready,值为1就代表缓存寄存器接收到数据(FIFO的前8位))
	{
	
		RecData2 = RdURXH0();	//将RdURXH0(0号接收缓存寄存器)中的数据读到RecData2中
		
		switch(RecData2)		//判断接收到的数据
		{
			case '1':
				rGPBDAT &= 	0xfffffe1f;		//(1110 0001 1111)如果数值为字符'1',就打开所有灯
				break;
			case '2':
				rGPBDAT |= 	0x1e0;  		//(0001 1110 0000)如果数值为字符'2',就关闭所有灯
				break;
			case '3':
				rGPBDAT |= 	0x01;  			//(0000 0000 0001)如果数值为字符'3',就打开beep
				break;
			case '4':
				rGPBDAT &= 	0xfffffffe;  	//(1111 1111 1110)如果数值为字符'4',就关闭beep
				break;
		}
	}
	
	ClearSubPending(BIT_SUB_RXD0);	//开启串口0接收子中断	
    ClearPending(BIT_UART0);		//开启串口0中断

	FlagRec = 0x01;					//将接受标记设定为1
}

void uprint(char *str)	//打印子程序
{
	while(*str)			//指针不为空的情况下循环
	{
		WrUTXH0(*str);	//将字符内容写出
		while( (rUTRSTAT0 & 0x04)==0);	//等待发送完成,UART接收发送状态寄存器只要为非空则进行空转,为空则跳出
		str++;			//指到下一个字符
	}
}


int Moain()			//这里不叫main,而是Moain,说明main只是一个普通函数,汇编定义入口的一个约定俗成而已,其实可以是任何函数名
{
	unsigned char i;	//定义一个index
	ledbeepinit();		//初始化beep和led
	UART0Init();		//初始化UART0
	RecLen = 0;			//接收长度置零
	FlagRec = 0;		//接受标记置零
	uprint("hello");	//将"hello"打印出来
	while(1)			//无限循环
	{ 							
	 	if(FlagRec == 0x01)	//如果中断标记为1
		{
			WrUTXH0(RecData2);	//就将收到的数据写出
			while( (rUTRSTAT0 & 0x04)==0);	//等待发送完成,UART接收发送状态寄存器只要为非空则进行空转,为空则跳出
			FlagRec = 0x00;		//将中断标记清0
			uprint("KO");		//再打印"KO"作为提示
		}		  
	} 
}
