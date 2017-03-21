#include "2440addr.h"		//将"2440addr.h"包含进来,这里面放的是所有寄存器的地址宏


void KeyInit(void)			//Key的初始化函数
{
	rGPFCON = (2<<8) | (2<<4) | (2<<2) | (2<<0);	//将GPF{0,2,4,8}设定为中断模式
	rEXTINT0 = (0<<16) | (0<<8) | (0<<4) | (0<<0); 	//将EINT{0,1,2,4}设为000,表示低电位触发中断

	EnableIrq(BIT_EINT0);	//启用EINT0位的中断     
	EnableIrq(BIT_EINT1);	//启用EINT1位的中断
	EnableIrq(BIT_EINT2);	//启用EINT2位的中断		 
	EnableIrq(BIT_EINT4_7);	//启用EINT4_7位的中断
	rEINTMASK = 0xFFFEF; 	//开启外部中断屏蔽寄存器的EINT4位中断(11101111)

}

void LEDInit(void)	//LED的初始化函数
{
	rGPBCON = (1<<16) | (1<<14) | (1<<12) | (1<<10);	//将GPB{5,6,7,8}设定为输出
	rGPBDAT = (1<<8) | (1<<7) | (1<<6) | (1<<5);   		//将GPB{5,6,7,8}设定为高电位,也就是灭掉所有灯
}


void Handle_EINT0(void)				//EINT0的中断服务程序
{
	rGPBDAT = ~(0x100);		   		//(1110 1111 1111)将[8]位置0,LED4亮
	while(1)
	{
		if(rGPFDAT & 0x00000001)	//如果GPF0为高电位,K4按键复位	
		{
			rGPBDAT = 0x1E0;		//(0001 1110 0000)将GPB{5,6,7,8}设定为高电位,也就是灭掉所有灯
			ClearPending(BIT_EINT0);//清掉EINT0位的中断 
			return;
		}
	}
}

void Handle_EINT4_7(void)			//EINT4_7位的中断服务程序
{
	rGPBDAT = ~(0x40);				//(1111 1011 1111)将[6]位置0,LED2亮	
	while(1)
	{
		if(rGPFDAT & 0x00000010)	//如果GPF4为高电位,K2按键复位
		{
			rGPBDAT = 0x1E0;		//(0001 1110 0000)将GPB{5,6,7,8}设定为高电位,也就是灭掉所有灯
			rEINTPEND = (1<<4);		//清外中断标志EINT4,通过给高电位00010000来清中断标记
			ClearPending(BIT_EINT4_7);	//清掉BIT_EINT4_7位的中断
			return;
		}
	}
	
}

void Handle_EINT1(void)		   		//EINT1位的中断服务程序
{
	rGPBDAT = ~(0x20);	   			//(1111 1101 1111)将[5]位置0,LED1亮    
	while(1)
	{
		if(rGPFDAT & 0x02)			//(0010)如果GPF1为高电位,K1按键复位
		{
			rGPBDAT = 0x1E0;     	//(0001 1110 0000)将GPB{5,6,7,8}设定为高电位,也就是灭掉所有灯
			 ClearPending(BIT_EINT1);	//清掉EINT1位的中断 rSRCPND = 1<<1	   rINTPND = 1<<1;
			 return;
		}
	}
	
}

void Handle_EINT2(void)			  	//EINT2位的中断服务程序
{
	rGPBDAT = ~(0x80);			  	//(1111 0111 1111)将[7]位置0,LED3亮 
	while(1)
	{
		if(rGPFDAT & 0x00000004)  	//(0100)如果GPF2为高电位,K3按键复位
		{
			rGPBDAT = 0x1E0;		//(0001 1110 0000)将GPB{5,6,7,8}设定为高电位,也就是灭掉所有灯
			ClearPending(BIT_EINT2);//清掉EINT2位的中断	
			return;
		}
	}
	
}

int Main()		//主函数,注意这里是Main,而不是main,故意为之,以展示入口函数的自定义性
{
	LEDInit();	//LED的初始化
	KeyInit();	//Key的初始化

	while(1);	//无限循环
}
