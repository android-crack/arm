#include "2440addr.h"					//��"2440addr.h"��������,������ŵ������мĴ����ĵ�ַ��

void UART0Init(void)					//��ʼ��UART0
{
	rGPHCON = (2<<6) | (2<<4);			//�趨GPH{2,3}ΪTXD[0]��RXD[0]
	rGPHUP  = (1<<3) | (1<<2);			//��GPH{2,3}�������������
	
	rUBRDIV0= 658;					//�����ʳ����Ĵ���,UBRDIVn=(int)(UART_clock/(buad_rate*16))-1, ���ﲨ����	9600  PCLK=101.25M ;   101.25M/(9600*16) - 1 = 658
	
	rULCON0 = (0<<6) | (0<<3) | (0<<2) | (3<<0);	//���ͽ���ÿ֡������λ��Ϊ8bit,ÿ֡һ��ֹͣλ,û����żУ�飬����ģʽ����(�Ǻ���ģʽ),��0��ͨ�������Կ��ƼĴ����趨Ϊ(0000 0011)
	rUCON0  = (0<<12) | (0<<10) | (0<<9) | (0<<8) | (0<<7) | (0<<6) | (0<<5) | (0<<4) | (1<<2) | (1<<0);	//(0000 0101) UART���ƼĴ���,�趨Ϊ�շ��ж�ģʽ
	rUFCON0 = (0<<6) | (0<<4) | (0<<2) | (0<<1) | (0<<0);	//(0000)����fifo����Ϊ��,��ȡfifo����Ϊ1�ֽ�,����fifo���ú��Զ����,��ȡfifo���ú��Զ����,����fifo����
	rUMCON0 = (0<<4);				//����Modem���ƼĴ���,�ر�AFC(Auto Flow Control),ʹ�øߵ�ƽȥ����nRTS('H' level)
}

void RTC_Time_Set( void )				//�趨ʱ�亯��
{
	rRTCCON = 1 ;					//RTCCON��RTCEN����1��,�ͽ�����BCD�Ĵ����Ķ�д����

	rBCDYEAR = 0x15 ;				//�����Ϊ2015��
	rBCDMON  = 0x09 ;				//�·���Ϊ09��
	rBCDDATE = 0x28 ;				//������Ϊ28��	
	rBCDDAY  = 0x05 ;				//������Ϊ������
	rBCDHOUR = 0x14 ;				//ʱ����Ϊ14Сʱ
	rBCDMIN  = 0x52 ;				//������Ϊ52��
	rBCDSEC  = 0x27 ;				//������Ϊ27��
	
	rRTCCON &= ~1 ;					//RTCCON��RTCEN����0��,����ס��BCD�Ĵ����Ķ�д����
}


void Delay(unsigned long Cnt)				//��ʱ����
{
 	while(Cnt--);
}

int main()
{
	unsigned char year,month,day,hour,minute,second,oldsecond;	//����������ʱ����;���Ĵ�Ŵ�

	UART0Init();					//��ʼ��UART0
	RTC_Time_Set();					//�趨ʱ��

	rRTCCON = 1 ;					//RTCCON��RTCEN����1��,�ͽ�����BCD�Ĵ����Ķ�д����
	oldsecond = rBCDSEC  ;				//��ԭ��ʱ�䱣��
	rRTCCON &= ~1 ;					//RTCCON��RTCEN����0��,����ס��BCD�Ĵ����Ķ�д����

	while(1)					//����ѭ��
	{
		rRTCCON = 1 ;				//RTCCON��RTCEN����1��,�ͽ�����BCD�Ĵ����Ķ�д����

		year   = rBCDYEAR  ;			//������
		month  = rBCDMON  ;			//������
		day    = rBCDDATE  ;			//������	
		hour   = rBCDHOUR  ;			//����Сʱ
		minute = rBCDMIN  ;			//�����
		second = rBCDSEC  ;			//������
		
		rRTCCON &= ~1 ;				//RTCCON��RTCEN����0��,����ס��BCD�Ĵ����Ķ�д����
		if(second != oldsecond)			//�����������һֱ��Ϊ��
		{
			oldsecond =  second;		//����������
			WrUTXH0(0x32);	  		//дһ��'2'
			Delay(0x50000);			//��ʱ
			WrUTXH0(0x30);			//дһ��'0'
			Delay(0x50000);
			WrUTXH0(0x30 + (year >> 4));	//д�����ʮλ
			Delay(0x50000);
			WrUTXH0(0x30 + (year & 0x0F));	//д����ĸ�λ
			Delay(0x50000);
			WrUTXH0(0x30 + (month >> 4));	//д���µ�ʮλ	
			Delay(0x50000);
			WrUTXH0(0x30 + (month & 0x0F));	//д���µĸ�λ
			Delay(0x50000);
			WrUTXH0(0x30 + (day >> 4));	//д���յ�ʮλ
			Delay(0x50000);
			WrUTXH0(0x30 + (day & 0x0F));	//д���յĸ�λ
			Delay(0x50000);
			WrUTXH0(0x30 + (hour >> 4));	//д��ʱ��ʮλ
			Delay(0x50000);
			WrUTXH0(0x30 + (hour & 0x0F));	//д��ʱ�ĸ�λ
			Delay(0x50000);
			WrUTXH0(0x30 + (minute >> 4));	//д���ֵ�ʮλ
			Delay(0x50000);
			WrUTXH0(0x30 + (minute & 0x0F));//д���ֵĸ�λ
			Delay(0x50000);
			WrUTXH0(0x30 + (second >> 4));	//д�����ʮλ
			Delay(0x50000);
			WrUTXH0(0x30 + (second & 0x0F));//д����ĸ�λ
			Delay(0x50000);
			
			WrUTXH0(0x0A);			//д��'\n'
		}
	}
}
