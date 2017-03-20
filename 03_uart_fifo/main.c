#include "2440addr.h" //将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏
//UART的fifo模式,先收再发8字节
	      
unsigned char FlagRec = 0x00; 	//定义一个标志,进行中断状态的记录与区别
unsigned char RecData[64], RecLen = 0;  //定义一个缓存区,和一个长度记录的变量

void UART0Init(void)			//初始化UART0
{
	rGPHCON = (2<<6) | (2<<4);	//配置为串口收发,在这个板子上GPH2:10代表TXD[0],GPH3:10代表RXD[0]
	rGPHUP  = (1<<3) | (1<<2);	//将GPH2和GPH3的上拉电阻禁用掉
	
	rULCON0 = 0x03;				//发送接收每帧的数据位数为8bit,每帧一个停止位,没有奇偶校验，正常模式操作(非红外模式),将0号通道的线性控制寄存器设定为(0000 0011)
	rUCON0  = 0x05; 			//(0000 0101),UART控制寄存器,设定为收发中断模式
	rUFCON0 = (0<<6) | (1<<4) | (1<<2) | (1<<1) | (1<<0); //(0001 0111)发送fifo触发为空,读取fifo触发为8字节,发送fifo重置后自动清除,读取fifo重置后自动清除,开启fifo特性
	rUMCON0 = (0<<4);			//串行Modem控制寄存器,关闭AFC(Auto Flow Control),使用高电平去激活nRTS('H' level)

	rUBRDIV0= 658;				//波特率除数寄存器,UBRDIVn=(int)(UART_clock/(buad_rate*16))-1, 这里波特率	9600  PCLK=101.25M ;   101.25M/(9600*16) - 1 = 658
	
	EnableSubIrq(BIT_SUB_RXD0);	//开启串口0接收子中断
	EnableIrq(BIT_UART0);	 	//开启串口0中断
}


void Handle_UART0(void) 
{
	unsigned long fifoleft; 	
	  
	while(1)		//无限循环
	{
		RecData[RecLen++] = RdURXH0();	//将RdURXH0(0号接收缓存寄存器)中的数据读到RecData数组中
		fifoleft = rUFSTAT0 & 0x0f;		//将FIFO状态寄存器中的低4位(FIFO中数据数量)取出,存到fifoleft
		if(fifoleft == 0)				//如果fifo中已经没有数据了
		{
			FlagRec = 0x01;				//将FlagRec置1
		    ClearSubPending(BIT_SUB_RXD0);	//清掉串口0接收子中断
            ClearPending(BIT_UART0);	//清掉串口0中断
			return;	
		}
	}	 	
}

int main()
{
	unsigned char i; 
	
	UART0Init(); 	//初始化UART0

	while(1)		//无限循环
	{
	 	if(FlagRec == 0x01)	//如果接收标志FlagRec为1
		{
			i = 0;
			while(1)	
			{	
				WrUTXH0(RecData[i++]);	//将RecData中的数据依次写出
				while( (rUTRSTAT0 & 0x04) == 0 );	//等待发送完成,UART接收发送状态寄存器只要为非空则进行空转,为空则跳出
		    	if(i >= RecLen)			//如果发送下标与RecLen齐平
				{
					RecLen = 0;			//就将RecLen接受长度置0
					FlagRec = 0x00;		//并且将接收标记置0
					break;				//并且跳出
				}	   		
			}	   
		}
	}
}
