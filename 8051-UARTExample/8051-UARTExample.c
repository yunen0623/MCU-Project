#include <reg51.h>
#define DataLength 8
unsigned char TxData[]={0x48,0x65,0x6c,0x6c,0x6F,0x21,0x0D,0x0A}; //Hello
unsigned char iTxCount = 1; //initialization since Hello[0] is sent before ISR
//char c='Hello';
main()
{
	SCON=0x50; 
	PCON &= 0X7F; //SMOD=0
	TMOD |= 0X20; //Timer1模式2
	TH1=TL1=0XFD; //鮑率9600
	IE = 0X90;
	TR1 =1; 
	SBUF = TxData[0]; //產生Tx中斷，並進入UART之ISR
	while(1);
}

void UARTInt(void) interrupt 4
{
	if(TI)
	{
		TI=0;//Reset T1
		iTxCount++;
		if(iTxCount == DataLength) //Repeat
		{
			iTxCount = 0; //Check and repeatly Tx
		}
		SBUF = TxData[iTxCount];
	}
}