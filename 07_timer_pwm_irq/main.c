#include "2440addr.h"			//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏

int i;							//定义一个全局的计数器,用来数中断个数

void Timer0Init(void)			//timer0的初始化程序
{
    rTCFG0  = 124;	   			//TCFG0(定时器配置寄存器0) 设定timer0预标定器值为124,定时器0和1共享一个8位的预分频器(预定标器),定时器2,3,4共享一个8位预分频器(预定标器)
    rTCFG1  = 0x03;		  		//(0000 0011)TCFG1(定时器配置寄存器1) 设定timer0的分频值为 1/16分频,每个定时器有一个时钟分频器,其可以生成5种不同的分频信号(1/2,1/4,1/8,1/16和TCLK)
								//定时器输出时钟频率 = PCLK/{prescaler + 1}/{divider}
								//timer0 = PCLK/(124+1)/16 = 101.25MHz/125/16 = 50625Hz
    rTCNTB0 = 50000;	   		//timer0定时器计数缓存寄存器设定值为50000(这个值不能超过65535) 50000*1/50625Hz = 0.987654320s ~ 1s
	rTCMPB0 = 10000;			//timer0定时器的比较缓存寄存器
	
	rTCON   = (1<<3)|(0<<2)|(1<<1)|(0<<0);	//TCON定时器控制寄存器, timer0 启用 auto reload, 禁用反相器, 手动加载一次 TCNTB 和 TCMPB ,禁用timer0
	rTCON   = (1<<3)|(0<<2)|(0<<1)|(1<<0);	//TCON定时器控制寄存器, timer0 启用 auto reload, 禁用反相器, 不用手动加载 TCNTB 和 TCMPB(前面一个操作手动加载了) ,开启timer0
											
  	EnableIrq(BIT_TIMER0);					//开中断，直接用PWM驱动beep,此时中断服务程序里面可以改变rTCMPB0，从而改变占空比
}


void BuzzerInit(void)			//蜂鸣器的初始化函数
{
	rGPBCON = (2 << 0);	 		//(0x00000002)buzzer连接到GPB0,内部的TOUT0也连接到GPB0,这个设置就是将GPB0设为TOUT0模式,那么buzzer就与timer0联通了
	i = 0;		  				//将i置0
}



void hander_timer0()	   		//hander_timer0中断服务程序
{
	i++;						//将中断计数器加1
	if(i>4)i=0;					//如果i超过4了(一旦涨到5了)就置0
	rTCMPB0 = 10000+i*10000;	//改变定时器0的计数比较缓存寄存器值(修改频宽)
     
	ClearPending(BIT_TIMER0);	//清理timer0的中断
}

int main()						//主函数,这里是kain,而不是main,故意为之,以展示入口函数的自定义性
{
	BuzzerInit();				//蜂鸣器的初始化
	Timer0Init();				//timer0的初始化

	while(1);					//无限循环
}
