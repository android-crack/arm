#include "2440addr.h"				//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏

//查询方式使用ADC					

void UART0Init(void)				//初始化UART0
{
	rGPHCON = (2<<6) | (2<<4);		//设定GPH{2,3}为TXD[0]和RXD[0]
	rGPHUP  = (1<<3) | (1<<2);		//将GPH{2,3}的上拉电阻禁用
	
	rUBRDIV0= 658;				//波特率除数寄存器,UBRDIVn=(int)(UART_clock/(buad_rate*16))-1, 这里波特率	9600  PCLK=101.25M ;   101.25M/(9600*16) - 1 = 658
	
	rULCON0 = (0<<6) | (0<<3) | (0<<2) | (3<<0);		//发送接收每帧的数据位数为8bit,每帧一个停止位,没有奇偶校验，正常模式操作(非红外模式),将0号通道的线性控制寄存器设定为(0000 0011)
	rUCON0  = (0<<12) | (0<<10) | (0<<9) | (0<<8) | (0<<7) | (0<<6) | (0<<5) | (0<<4) | (1<<2) | (1<<0);	//(0000 0101) UART控制寄存器,设定为收发中断模式
	rUFCON0 = (0<<6) | (0<<4) | (0<<2) | (0<<1) | (0<<0);	//(0000)发送fifo触发为空,读取fifo触发为1字节,发送fifo重置后不自动清除,读取fifo重置后不自动清除,禁用fifo特性
	rUMCON0 = (0<<4);			//串行Modem控制寄存器,关闭AFC(Auto Flow Control),使用高电平去激活nRTS('H' level)
}

void ADCInit(void)				//ADC初始化程序
{						//ADC控制寄存器
						//AD转换器预分频器(预定标器)使能，AD转换器预分频值为200，模拟输入通道选择为AIN2,普通操作模式,通过读取操作开始无效,先不开始AD转换
	rADCCON = (1 << 14) | (200 << 6) | (2 << 3) | (0 << 2) | (0 << 1) | (0 << 0);
}

void Delay(unsigned long Cnt)			//延时程序
{
 	while(Cnt--);
}

int main()	
{
	unsigned short adc_result;		//定义adc结果的存放处
	unsigned char qw, bw, sw, gw;		//定义千/百/十/个位的值存变量

	UART0Init();				//初始化UART0
	ADCInit();				//初始化ADC
	Delay(500);				//进行延时
	
	while(1)
	{
		rADCCON |= 0x01;		//开启AD转换，并且在开始后启动位清零
		 
		while(!(rADCCON & 0x8000));	//查询是否转换完成,如未完成就继续等待(空转)

		adc_result =  rADCDAT0 & 0x3ff;	//ADC转换数据寄存器0,ADCDAT0[0:9]共10位为转换数值,范围是(0-1023) x=3.3/1024*adc_result

		qw = adc_result / 1000;		//对1000整除,将千位取出存到qw中
		adc_result %= 1000;		//对1000取模,去掉千位以上的部分,余下的百十个位值存回

		bw = adc_result / 100;		//对100整除,将百位取出存到bw中
		adc_result %= 100;		//对100取模,去掉百位以上的部分,余下的十个位值存回

		sw = adc_result / 10;		//对10整除,将十位取出存到sw中
		gw = adc_result % 10;		//对10取模,去掉十位以上的部分,余下的个位值存回
	
		WrUTXH0(0x30 + qw);		//写出千位
		while(!(rUTRSTAT0 & 0x2));	//检查是否发送完成,如未完成就继续等待(空转)

		WrUTXH0(0x30 + bw);		//写出百位		
		while(!(rUTRSTAT0 & 0x2));	//等待发送完成

		WrUTXH0(0x30 + sw);		//写出十位
		while(!(rUTRSTAT0 & 0x2));	//等待发送完成

		WrUTXH0(0x30 + gw);		//写出个位
		while(!(rUTRSTAT0 & 0x2));	//等待发送完成

		WrUTXH0(0x0A);			//写出'\n'		
		while(!(rUTRSTAT0 & 0x2));	//等待发送完成

		Delay(50000);
	}
}
