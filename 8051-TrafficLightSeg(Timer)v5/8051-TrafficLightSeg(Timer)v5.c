#include <reg51.h>
#define SEG P2
sbit LEDG=P1^0;
sbit LEDY=P1^1;
sbit LEDR=P1^2;
sbit LEDG1=P1^3;
sbit LEDY1=P1^4;
sbit LEDR1=P1^5;
unsigned char count;
char  secY[3]={0xc0,0xf9,0xa4}; //1-2 redlight sec
char  secR[3]={0xb0,0x99,0x92}; //3-5 redlight sec
int t=0; //計算紅綠燈秒數
int i;

main()
{
	IE=0x88;//TIMER1開啟
	TMOD=0x10;  //設定TIMER1
	TH1=(65536-46080)/256; //設定TIMER1 約0.05s
	TL1=(65536-46080)%256;
	TR1=1;//開始計時
	while(1)
	{
		if(t>0 && t<=3)
		{
			LEDG=0;
			LEDY=1;
			LEDR=1;
			LEDG1=1;
			LEDY1=1;
			LEDR1=0;
			{
				for(i=2;i>=0;)
				P2=secR[i];
				//SEG=0XFF;
			}
		}
		else if(t>3 && t<=5)
		{
			LEDG=1;
			LEDY=0;
			LEDR=1;
			LEDG1=1;
			LEDY1=1;
			LEDR1=0;
			{
				for(i=2;i>=0;)
				P2=secY[i];
				//SEG=0XFF;
			}
		}
		else if(t>5 && t<=8)
		{
			LEDG=1;
			LEDY=1;
			LEDR=0;
			LEDG1=0;
			LEDY1=1;
			LEDR1=1;
			{
				for(i=2;i>=0;)
				P2=secR[i];
				//SEG=0XFF;
			}
		}
		else if(t>8 && t<=10)
		{
			LEDG=1;
			LEDY=1;
			LEDR=0;
			LEDG1=1;
			LEDY1=0;
			LEDR1=1;
			{
				for(i=2;i>=0;)
				P2=secY[i];
				//SEG=0XFF;
			}
		}
	}
}

void Timer1Int(void) interrupt 3
{
	TH1=(65536-46080)/256; //??TIMER1 ?0.05s
	TL1=(65536-46080)%256;
	if(++count>=20) //1s
	{
		count=0;
		i--;
		t+=1;// t=t+1	
		if(t>11)
		{
			t=0;
		}
	}
}
