#include "2440addr.h"				//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏

volatile int  i=0,j=0;				//定义两个全局变量

void wtdInit(void)					//初始化看门狗
{

	rWTCON   = (124<<8) | (0<<5) | (1<<2) | (0<<0);	//预定标器值为124(最大可设为255),看门狗定时器禁用,除数因子为16,中断启用,看门狗定时器复位功能设为无效,t_watchdog=1/(PCLK/(Prescaler_value + 1 )/Division_factor) = 1/(101.25M/(124+1)/16) = 1/(101.25*10^6/125/16)=1/50625 s				
	rWTDAT   = 50625; 		//看门狗定时器数据寄存器设定为50625,也就是1s钟 (这个值要小于65535)
	rWTCNT   = 50625; 		//看门狗定时器计数寄存器设定为50625
	EnableIrq(BIT_WDT_AC97);//开启看门狗中断
	EnableSubIrq(BIT_SUB_WDT);//开启看门狗子中断
	rWTCON   |=   (1<<5);	//启用看门狗			  
	
	j = 0; 					//将j置0
}


void BuzzerInit(void)				//初始化蜂鸣器
{
	rGPBCON = (1 << 0);	 			//将GPB0设定为输出模式	
	rGPBDAT = (0 << 0);				//将GPB0设定为高电位,关闭蜂鸣器
	i = 0;	  						//将i置0
}



void hander_wtd()	   				//看门狗中断服务程序
{
	j++;							//使用j计数

	if(j==2)						//如果j为2
	{
		rGPBDAT = rGPBDAT^1;		//将GPB0反转
		j = 0;						//将j置0
	}
	
	ClearPending(BIT_WDT_AC97);		//清看门狗中断
	ClearSubPending(BIT_SUB_WDT);	//清看门狗子中断
}

int main()
{
	BuzzerInit();					//初始化蜂鸣器
	wtdInit();						//初始化看门狗

	while(1);						//无限循环(等待中断产生)
}
