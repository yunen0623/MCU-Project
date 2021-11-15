#include <reg51.h>
#include <stdio.h>
#include <string.h>
#define LED P1
#define DataLengthg 7
#define DataLengthy 8
#define DataLengthr 5
#define DataLengths 6
void delay1ms(int x);
unsigned char green[ ]={0x47,0x72,0x65,0x65,0x6e,0x0D,0x0A}; //Green16秈
unsigned char yellow[ ]={0x59,0x65,0x6c,0x6c,0x6f,0x77,0x0D,0x0A}; //Yellow
unsigned char red[ ]={0x52,0x65,0x64,0x0D,0x0A};  //Red
unsigned char stop[ ]={0x53,0x74,0x6f,0x70,0x0D,0x0A}; //Stop
unsigned char ArrayCount = 0;
unsigned char iCount=0;
//unsigned char DataLength;
unsigned char IfTx=0;
char c,a=0;
int t,i,b;
main()
{
	IE=0x93; // INT0 UART TIMER0
	IP=0x10; //UARTㄣΤ程蔼纔舦
	IT0=0; //璽絫い耞
	//-------------------Timer0 set-----------------------//
	TMOD=0x21; //Timer0 gate c/t mode1    //TIMER 家Α2
	TH0=(65536-46080)/256;
	TL0=(65536-46080)%256;
	//-------------------UARR set------------------------//
	SCON=0x50; //﹃家Α1
	PCON &= 0x7F; //SMOD=0
	TH1=TL1=0xFD; //纉瞯9600
	//TMOD |= 0X20; //Timer1家Α2
	//---------------------------------------------------//
	TR0=1; //T0 い耞秨﹍
	TR1 =1; // UART TIMER1秨﹍
	while(1);
}

void INT0Int(void) interrupt 0
{
	int i;
	for(i=0;i<5;i++)
	{
		LED=0xED;
		delay1ms(250);
		LED=0xFF;
		delay1ms(250);
		//i++;
	}
	//i=0;
}

void Timer0Int(void) interrupt 1
{
	TH0=(65536-46080)/256;
	TL0=(65536-4608)%256; //0.05S
	if(++iCount>=20) //1s
	{
		iCount=0;
		t++;
		//b++;
	}
	switch (t)
	{
		case(0):
			LED=0xDE;
			break;
		case(3):
			LED=0xDD;
			break;
		case(5):
			LED=0xF3;
			break;
		case(8):
			LED=0xEB;
			break;
	}
	if(t>=10)
	{
		t=0;
		//b=0;
	}
}

void UARTInt(void) interrupt 4
{
	if(RI)
	{
		RI=0;
		c=SBUF;
		switch(c)
		{
			case 'g':
				P1=0xDE;
				IfTx=1;
				SBUF=green[0];
				break;
			case 'y':
				P1=0xED;
				IfTx=1;
				SBUF=yellow[0];
				break;
			case 'r':
				P1=0xF3;
				IfTx=1;
				SBUF=red[0];
				break;
			case 's':
				P1=0xFF;
				IfTx=1;
				SBUF=stop[0];
				break;
		}
		
	}
	
	if(TI)
	{
		TI=0;
		if(IfTx==0) return;
		switch(c)
		{
			case 'g':
				ArrayCount++;
					if(ArrayCount >= DataLengthg) //Repeat
					{
						ArrayCount = 0; //Check and repeatly Tx
						IfTx=0;
					}
					else
						SBUF = green[ArrayCount];
					break;
			case 'y' :
				ArrayCount++;
					if(ArrayCount >= DataLengthy) //Repeat
					{
						ArrayCount = 0; //Check and repeatly Tx
						IfTx=0;
					}
					else
						SBUF = yellow[ArrayCount];
					break;
			case 'r' :
				ArrayCount++;
					if(ArrayCount >= DataLengthr) //Repeat
					{
						ArrayCount = 0; //Check and repeatly Tx
						IfTx=0;
					}
					else
						SBUF = red[ArrayCount];
					break;
			case 's' :
				ArrayCount++;
					if(ArrayCount >= DataLengths) //Repeat
					{
						ArrayCount = 0; //Check and repeatly Tx
						IfTx =0;
					}
					else
						SBUF = stop[ArrayCount];
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
