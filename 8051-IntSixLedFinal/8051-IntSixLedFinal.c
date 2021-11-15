#include <reg51.h>
#define LED P1
void delay1ms(int);
void runled(int);
//void debouncer(void);
main()
{
	IE = 0x81;
	IT0 = 0;
	P1 = 0xF8;
	while(1)
	{
		delay1ms(250); //0.25s 
		P1 = ~P1;
	}
}

void INT0Int(void) interrupt 0
{
	unsigned saveLED=LED;
	runled(3);
	LED = saveLED;
}

void runled(int x)
{
	int i ;
	unsigned char j;
	P1=0xFE;
		for(j=0;j<x;j++)
		{
			for(i=0;i<30000;i++);
			P1=(P1<<1)|0x01;
		}
		for(j=0;j<x;j++)
		{
			for(i=0;i<30000;i++);
			P1=(P1>>1)|0x80;
		}
		delay1ms(250);
}

void delay1ms(int x)
{
	int i,j;
	for(i=0;i<x;i++)
	for(j=0;j<250;j++);
}

/*void debouncer(void)
{
	int i;
	for(i=0;i<2400;i++);
}*/