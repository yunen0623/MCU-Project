#include <reg51.h>
#define LED P1
void delay1(long t);
void delay2(int x);
char light[]={0XFE,0XFD,0XFB};
char time[]={10,8,3};
unsigned char iCount=0;
main()
{
	IE=0x82;
	TMOD=1;
	TH0=(65535-46080)/256;
	TL0=(65536-46080)%256;
	TR0=1;
	P1=0XFD;
	delay2(30000);
	while(1);
}

void Timer0Int(void) interrupt 1
{
	int i;
	TH1=(65535-46080)/256;
	TL1=(65536-46080)%256;
	if(++iCount==60)
	{
		iCount=0;
		for(i=0;i<3;i++)
		{
			P1=light[i];
			delay1(time[i]);
		}
	}
}
void delay1(long t)
{
	t=t*2000;
	while(t>0)
		t--;
}

void delay2(int x)
{
	int i;
	for(i=0;i<x;i++);
}
