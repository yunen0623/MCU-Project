#include <reg51.h>
#include <stdio.h>
#include <string.h>
#define LED P1
void delay1ms(int x);
unsigned char iCount=0;
char c,a=0;
int t,i;
main()
{
	IE=0x93; // INT0 UART TIMER0
	IP=0x10; //UART�㦳�̰��u���v
	IT0=0; //�t�t���_
	//-------------------Timer0 set-----------------------//
	TMOD=0x21; //Timer0 gate c/t mode1    //TIMER �Ҧ�2
	TH0=(65536-46080)/256;
	TL0=(65536-46080)%256;
	//-------------------UARR set------------------------//
	SCON=0x50; //��C�Ҧ�1
	PCON &= 0x7F; //SMOD=0
	TH1=TL1=0xFD; //�j�v9600
	//TMOD |= 0X20; //Timer1�Ҧ�2
	//---------------------------------------------------//
	TR0=1; //T0 ���_�}�l
	TR1 =1; // UART TIMER1�}�l
	while(1);
}

void INT0Int(void) interrupt 0
{
	int i;
	while(i<5)
	{
		LED=0xED;
		delay1ms(250);
		LED=0xFF;
		delay1ms(250);
		i++;
	}
	i=0;
}

void Timer0Int(void) interrupt 1
{
	TH0=(65536-46080)/256;
	TL0=(65536-4608)%256; //0.05S
	if(++iCount>=20) //��1s
	{
		iCount=0;
		t++;
	}
	switch (t)
	{
		case(0):
			LED=0xDE;
			break;
		case(5):
			LED=0xDD;
			break;
		case(8):
			LED=0xF3;
			break;
		case(10):
			LED=0xEB;
			break;
	}
	if(t>=12)
	{
		t=0;
	}
}

void UARTInt(void) interrupt 4
{
	
	if (RI==1)
	{
	RI=0;
	c=SBUF;
	switch(c)
	{
		case 'g':
			P1=0xDE;
			TI=1;
			printf ("green"); 
			break;

		case 'y':
			P1=0xED;
			TI=1;
			printf ("yellow");
			break;

		case 'r':
			P1=0xF3;
			TI=1;
			printf ("red");
			break;

		case 's':
			P1=0xFF;
			TI=1;
			printf ("stop");
			break;
		}
	}
}

void delay1ms(int x)
{
	int i,j;
	for(i=0;i<x;i++)
	for(j=0;j<120;j++);
}
