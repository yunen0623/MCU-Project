#include <reg51.h>
#include <stdio.h>
#include <string.h>
char c;
//unsigned char i;
void UART_int(void)interrupt 4
{
	c=0;
	do
	{
		while(!RI); 
		RI = 0;
		if ( SBUF == 0x00 )
		break;
		c = SBUF;
	}
	while( c < 2);

	if (RI==1); 
	RI=0;
	{
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
main()
{
	SCON=0x50; 
	PCON &= 0X7F; //SMOD=0
	TMOD |= 0X20; //Timer1模式2
	TH1=TL1=0XFD; //鮑率9600
	IE = 0X90;
	TR1 =1; 
	//SBUF = TxData[0]; //產生Tx中斷，並進入UART之ISR
	while(1);
}
