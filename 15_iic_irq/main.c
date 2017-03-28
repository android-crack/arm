#include "2440addr.h"					//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏	

unsigned char f_nGetACK;			 	//响应全局变量

void IICInit(void)					//初始化IIC
{
	f_nGetACK = 0;					//置0

	rGPECON = (2 << 28) | (2 << 30);	  	//配置IIC引脚,GPECON[29:28]=10, GPECON[31:30]=10就代表 GPE14:IICSCL,GPE15:IICSDA ,这两个针脚没有上拉电阻,所以需要额外提供上拉
	rGPEUP |= 0xc000;				//上拉的默认值(其实可以不要)
   		 
    	rIICCON = (1 << 7) | (0 << 6) | (1 << 5) | (0 << 4) | (15);	//IIC总线应答启用, 传输时钟预定标器源时针钟选择位为 IICCLK=fPCLK/16,接收发送中断启用,接收发送中断挂起标志,发送时钟预定标器的值为15(时钟频率 Tx clock=IICCLK/(IICCON[3:0]+1)=fPCLK/16/(15+1)=fPCLK/256)=101.25M/256
    	rIICSTAT = (1 << 4);				//IIC总线控制状态寄存器设定,从接收模式,读不忙,数据输出使能位,总线仲裁成功,address-as-slave状态标志位清0,地址0状态标志位清0,最后收到位状态标志位清0(最后收到位是0 ACK位)
	
	rINTMSK &= ~BIT_IIC;				//启用IIC中断,相当于EnableIrq(BIT_IIC);		 
}

void UART0Init(void)					//初始化UART0
{
	rGPHCON = (2<<6) | (2<<4);			//设定GPH{2,3}为TXD[0]和RXD[0]
	rGPHUP  = (1<<3) | (1<<2);			//将GPH{2,3}的上拉电阻禁用
	
	rUBRDIV0= 658;					//波特率除数寄存器,UBRDIVn=(int)(UART_clock/(buad_rate*16))-1, 这里波特率	9600  PCLK=101.25M ;   101.25M/(9600*16) - 1 = 658
	
	rULCON0 = (0<<6) | (0<<3) | (0<<2) | (3<<0);	//发送接收每帧的数据位数为8bit,每帧一个停止位,没有奇偶校验，正常模式操作(非红外模式),将0号通道的线性控制寄存器设定为(0000 0011)
	rUCON0  = (0<<12) | (0<<10) | (0<<9) | (0<<8) | (0<<7) | (0<<6) | (0<<5) | (0<<4) | (1<<2) | (1<<0);	//(0000 0101) UART控制寄存器,设定为收发中断模式
	rUFCON0 = (0<<6) | (0<<4) | (0<<2) | (0<<1) | (0<<0);	//(0000)发送fifo触发为空,读取fifo触发为1字节,发送fifo重置后不自动清除,读取fifo重置后不自动清除,禁用fifo特性
	rUMCON0 = (0<<4);				//串行Modem控制寄存器,关闭AFC(Auto Flow Control),使用高电平去激活nRTS('H' level)
}

void Delay(unsigned long Cnt)				//延时程序
{
 	while(Cnt--);
}


//主发模式
void iic_write_24c040(unsigned long unSlaveAddr,unsigned long unAddr,unsigned char ucData)
{
	f_nGetACK = 0;
    
    	//发送控制字符
    	rIICDS = unSlaveAddr;				//0xa0(1010 0000)[4+3+1],1010为IIC的协议地址(已经被约定好了),000为A[0-2],是低电平,0是写模式
    	rIICSTAT = 0xf0;				//(1111 0000) 主发模式,读忙写开始(开始信号后,IICDS中的数据自动被传输),IIC总线数据输出使能(有效Rx/Tx),总线仲裁成功,最后收到位是0(收到ACK)

	//等待响应
    	while(f_nGetACK == 0);				//如果f_nGetACK为0就进行等待,直到f_nGetACK不为0(也就是收到了应答中断)
    	f_nGetACK = 0;					//将f_nGetACK置0
    
    	//发送读写地址
    	rIICDS = unAddr;				//依次写入的地址
    	rIICCON = 0xaf;					//(1010 1111)恢复IIC模式,IIC总线应答启用, 传输时钟预定标器源时针钟选择位为 IICCLK=fPCLK/16,接收发送中断启用,接收发送中断挂起标志,发送时钟预定标器的值为15(时钟频率 Tx clock=IICCLK/(IICCON[3:0]+1)=fPCLK/16/(15+1)=fPCLK/256)=101.25M/256
    
    	while(f_nGetACK == 0);				//如果f_nGetACK为0就进行等待,直到f_nGetACK不为0(也就是收到了应答中断)
    	f_nGetACK = 0;					//将f_nGetACK置0
    
    	//发送数据
    	rIICDS = ucData;				//将要发送出去的数据写到IICDS中
    	rIICCON = 0xaf;					//(1010 1111)恢复IIC模式
    
    	while(f_nGetACK == 0);				//等待应答
    	f_nGetACK = 0;					//将f_nGetACK置0
    
    	//结束发送
    	rIICSTAT = 0xd0;				//(1101 0000)主发模式,读不忙停止信号生成,IIC总线数据输出使能(有效Rx/Tx),总线仲裁成功,最后收到位是0(收到ACK)
    	rIICCON = 0xaf;					//(1010 1111)恢复IIC模式
    	Delay(50000);					//延时
}

//主收模式
void iic_read_24c040(unsigned long unSlaveAddr,unsigned long unAddr,unsigned char *pData)
{
	f_nGetACK = 0;					//应答标识位置0

    	//发送控制字符 
    	rIICDS = unSlaveAddr;				//(1010 0000) 0xa0(1010 0000)[4+3+1],1010为IIC的协议地址(已经被约定好了),000为A[0-2],是低电平,0是写模式
    	rIICSTAT = 0xf0;					//(1111 0000) 主发模式,读忙写开始(开始信号后,IICDS中的数据自动被传输),IIC总线数据输出使能(有效Rx/Tx),总线仲裁成功,最后收到位是0(收到ACK)

    	while(f_nGetACK == 0);				//等待应答
    	f_nGetACK = 0;					//将f_nGetACK置0

    							//发送读写地址
    	rIICDS = unAddr;				//依次写入的地址
    	rIICCON = 0xaf;					//(1010 1111)恢复IIC模式,IIC总线应答启用, 传输时钟预定标器源时针钟选择位为 IICCLK=fPCLK/16,接收发送中断启用,接收发送中断挂起标志,发送时钟预定标器的值为15(时钟频率 Tx clock=IICCLK/(IICCON[3:0]+1)=fPCLK/16/(15+1)=fPCLK/256)=101.25M/256
    
    	while(f_nGetACK == 0);				//等待应答
    	f_nGetACK = 0;					//将f_nGetACK置0

    
    	rIICDS = unSlaveAddr;				//(1010 0000) 0xa0(1010 0000)[4+3+1],1010为IIC的协议地址(已经被约定好了),000为A[0-2],是低电平,0是写模式
    	rIICSTAT = 0xb0;				//(1011 0000) 主接收模式,读忙写开始(开始信号后,IICDS中的数据自动被传输),IIC总线数据输出使能(有效Rx/Tx),总线仲裁成功,最后收到位是0(收到ACK)
    	rIICCON = 0xaf;					//(1010 1111)恢复IIC模式,IIC总线应答启用, 传输时钟预定标器源时针钟选择位为 IICCLK=fPCLK/16,接收发送中断启用,接收发送中断挂起标志,发送时钟预定标器的值为15(时钟频率 Tx clock=IICCLK/(IICCON[3:0]+1)=fPCLK/16/(15+1)=fPCLK/256)=101.25M/256   
	
    	while(f_nGetACK == 0);				//等待应答
    	f_nGetACK = 0;					//将f_nGetACK置0
    
    	//获取数据
    	rIICCON = 0x2f;					//(0010 1111),IIC总线应答禁用, 传输时钟预定标器源时针钟选择位为 IICCLK=fPCLK/16,接收发送中断启用,接收发送中断挂起标志,发送时钟预定标器的值为15(时钟频率 Tx clock=IICCLK/(IICCON[3:0]+1)=fPCLK/16/(15+1)=fPCLK/256)=101.25M/256
    	Delay(10000);					//适当延时
    
    	*pData = rIICDS;				//读取数据到*pData中
    
    	//中止传输
    	rIICSTAT = 0x90;				//(1001 0000)主接收模式,读不忙停止信号生成,IIC总线数据输出使能(有效Rx/Tx),总线仲裁成功,最后收到位是0(收到ACK) 
	rIICCON = 0xaf;					//(1010 1111)恢复IIC模式
	Delay(50000);					//适当延时
}

unsigned char	szData[256];				//定义一个长度为256的字符数组

int main()		
{
	unsigned char i=0;				//循环索引变量

	UART0Init();					//初始化UART0
	IICInit();					//初始化IIC

	while(1)					//无限循环
	{
		//Write 0 - 15 to 24C04
	    for(i=0; i<16; i++)				//遍历[0,15]		
	    	{
			iic_write_24c040(0xa0, i, i);	//挑选芯片(1010 000),在地址为i的位置写入值i
			Delay(1000);			//适当延时
		}
		
	    for(i=0; i<16; i++)	szData[i]=0;		//将数组的前16个位置清0
	    
	    // Read 16 byte from 24C04
	    for(i=0; i<16; i++)				//进行[0-15]的遍历
		{
			iic_read_24c040(0xa0, i, &(szData[i])); //从24c04中读取数据写到数组中

			if(szData[i] >=0 && szData[i]<=9) WrUTXH0(0x30+szData[i]);	   
			else if(szData[i] > 9 && szData[i] < 16) WrUTXH0(szData[i] + 'A'-10);
			else WrUTXH0('X');		//根据值的范围将其写出到串口中

			while(!(rUTRSTAT0 & 0x4));	//等待串口发送完成
		}	
	}
}

void iic_int_24c04(void)				//IIC的中断服务程序
{	 
    	f_nGetACK = 1;					//收到IIC中断后,给f_nGetACK置1
    	ClearPending(BIT_IIC);				//清掉IIC中断
}	