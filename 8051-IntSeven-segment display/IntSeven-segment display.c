#include <reg51.h>
#define LED P1
#define SEG P2
void delay1ms(int x);
char code TAB[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x83,0xf8,0x80,0x98}; //前4為0-4,後為5-9

main()
{
	int i;
	IE = 0X81;
	IT0 =0;
	while(1)
	{
		for(i=0;i<10;i++)
		{
			SEG=TAB[i]; //顯示0-9(上數)
			delay1ms(500);  //延遲500ms*1m=0.5s
		}
	}
}

void INT0Int(void) interrupt 0
{
	int i;
	for(i=9;i>=0;i--) //9-0(下數)
	{
		SEG=TAB[i];
		delay1ms(250);
		SEG=0XFF; //關閉七節顯示器
		delay1ms(250);
	}
}

void delay1ms(int x)
{
	int i,j;
	for(i=0;i<x;i++)
	for(j=0;j<120;j++);
}