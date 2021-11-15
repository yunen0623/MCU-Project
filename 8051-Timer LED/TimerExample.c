#include<reg51.h> 
#define  LED  P1 
unsigned char iCount=0;



main()
{
	IE=0x88; //開啟TIMER1中斷
	TMOD = 0x10; //GATE1=0  CT1=0 MODE1=1
	TH1=(65536-46080)/256; //0.05秒中斷1次
	TL1=(65536-46080)%256;
	{
		int i;  
		unsigned j;
		P1=0xFE;
			while(1)
			{
				for(j=0;j<7;j++)
				{
					for(i=0;i<30000;i++);
					P1=(P1<<1)|0x01;
				}
				for(j=0;j<7;j++)
				{
					for(i=0;i<30000;i++);
					P1=(P1>>1)|0x80;
				}
		TR1=1;
		while(1);
			}
	}
}


void Timer1Int (void) interrupt 3  //Timer1之ISR
{
	TH1=(65536-46080)/256;
	TL1=(65536-46080)%256;
		if(++iCount>=20) //20次中斷(0.05S/次)約1S
		{
			iCount=0; //重置中斷次數為0
			P1=~P1; //LED反相
		}
	}