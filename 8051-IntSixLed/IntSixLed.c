#include <reg51.h>
unsigned LED_P1=0xEA;
unsigned LED_P2=0xF5;
char a; //for button
main()
{
	IT1=1;   //­t½tÄ²µo
	IE=0x84; // OPEN INT1
	a=1;
	while(1)
	{
		if(a%2==0)
		{
			P1=LED_P1;
			P2=~LED_P2;
		}
		if(a%2!=0)
		{
			P1=~LED_P1;
			P2=LED_P2;
		}
	}
}


void INT1Int(void) interrupt 2
{
	a=!a;  //ÅÞ¿è¤Ï¦V
}
	
	