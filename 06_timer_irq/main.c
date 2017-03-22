#include "2440addr.h"	//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏

int i;

void Timer0Init(void)	//timer0的初始化程序
{
    rTCFG0  = 124;		//TCFG0(定时器配置寄存器0) 设定timer0预标定器值为124,定时器0和1共享一个8位的预分频器(预定标器),定时器2,3,4共享一个8位预分频器(预定标器)
    rTCFG1  = 0x01;		//(0000 0001)TCFG1(定时器配置寄存器1) 设定timer0的分频值为 1/4分频,每个定时器有一个时钟分频器,其可以生成5种不同的分频信号(1/2,1/4,1/8,1/16和TCLK)  
						//定时器输出时钟频率 = PCLK/{prescaler + 1}/{divider}
						//timer0 = PCLK/(124+1)/4 = 101.25MHz/125/4 = 202.5KHz
    rTCNTB0 = 20250;	//timer0定时器计数缓存寄存器设定值为20250(这个值不能超过65535) 20250*1/202.5KHz = 0.1s =100ms 
                                  
	rINTMSK &= ~(BIT_TIMER0);  //打开timer0中断,相当于EnableIrq(BIT_TIMER0);  
	 
	rTCON   = (1<<3)|(0<<2)|(1<<1)|(0<<0); //TCON定时器控制寄存器, timer0 启用 auto reload, 禁用反相器, 手动加载一次 TCNTB 和 TCMPB ,禁用timer0
	rTCON   = (1<<3)|(0<<2)|(0<<1)|(1<<0); //TCON定时器控制寄存器, timer0 启用 auto reload, 禁用反相器, 不用手动加载 TCNTB 和 TCMPB(前面一个操作手动加载了) ,开启timer0


}


void BuzzerInit(void)		//蜂鸣器的初始化函数
{
	rGPBCON = (1 << 0);		//buzzer连接到GPB0,先将其配置为输出模式(01)
	rGPBDAT = (0 << 0);		//将GPB0设定为低电位,关闭蜂鸣器
	i = 0;					//将i置0
}


void Handle_Timer0(void)	// Handle_Timer0中断服务程序
{
	i++;					//i++
	if(i==10)				//如果i的值为10(触发了10次timer0中断)
	{
		rGPBDAT = rGPBDAT ^ 0x01;	//翻转GPB0电位(如果蜂鸣器响,则变为不响,如果蜂鸣器不响,则变得响)
		i=0;				//将i置0
	}
	ClearPending(BIT_TIMER0);  		//清理TIMER0中断
}

int kain()				//主函数,这里是kain,而不是main,故意为之,以展示入口函数的自定义性
{
	BuzzerInit();		//蜂鸣器的初始化
	Timer0Init();		//timer0的初始化

	while(1);			//无限循环
}
