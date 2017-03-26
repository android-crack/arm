#include "2440addr.h"				//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏

void wtdInit(void)
{
	rWTCON   = (124<<8) | (0<<5) | (0<<3)| (0<<2) | (0<<0);	//预定标器值为124(最大可设为255),看门狗禁用,除数因子为16,中断禁用,t_watchdog=1/(PCLK/(Prescaler_value + 1 )/Division_factor) = 1/(101.25M/(124+1)/16) = 1/(101.25*10^6/125/16)=1/50625 s				
	rWTDAT   = 50625; 				//看门狗定时器数据寄存器设定为50625,也就是1s钟			
	rWTCNT   = 50625; 				//看门狗定时器计数寄存器设定为50625	
	rWTCON   |= (1<<5) |(1<<0);   	//启用看门狗,看门狗定时器超时,会发出s3c2440A复位信号
}

void BuzzerInit(void)				//初始化蜂鸣器
{
	rGPBCON = (1 << 0);	 			//将GPB0设定为输出模式
	rGPBDAT = (1 << 0);				//将GPB0设定为高电位,开启蜂鸣器
}

void LEDsInit(void)
{
	rGPBCON |= (1 << 10);	   		//GPB5设定为输出模式	
	rGPBDAT |= (0 << 5);			//GPB5设定为低电位,开启LED1
}

int main()
{
	BuzzerInit();				 	//初始化蜂鸣器
	LEDsInit();						//初始化LED
	wtdInit();						//初始化看门狗
	while(1)						//无限循环
	{
		int a;						//定义一个变量
		for(a=0;a<20000;a++);		//空转20000次
		rGPBDAT = rGPBDAT ^ 1;		//将GPB电位翻转
		rWTCNT   = 50625; 			//喂狗,如果不喂狗,超时就会复位
	}
}
