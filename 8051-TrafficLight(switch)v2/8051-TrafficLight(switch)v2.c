#include <reg51.h>
#define LED P1
unsigned char c=0;
unsigned char iCount;
//char light[]={0xFE,0xFD,0xFB};

main()
{
IE=0x88;
TMOD=0x10;
TH1=(65536-46080)/256;
TL1=(65536-46080)%256;
TR1=1;
while(1)
{
switch (c)	
{
	case (0):
	P1=0xFE;
	break;
	
	case (1):
	P1=0xFD;
	break;
	
	case (2):
	P1=0xFB;
	break;
	
	default:
	P1=0XFf;
	break;
	
}
}
}


void Timer1Int(void) interrupt 3
{
	TH1=(65536-46080)/256;
	TL1=(65536-46080)%256;
	if(++iCount>=20)
	{
		iCount=0;
		c++;
		if(c>=4)
		{
			c=0;
		}
	}
}

