#include <reg51.h>
#include <intrins.h>
sbit scl = P3^6;
sbit sda = P3^7;
unsigned char coding[8]={0x48,0x65,0x6c,0x6c,0x6F,0x21,0x0D,0x0A};//??0-F??

void Delay_10us(unsigned int time)
{
	while(time--);
}

void I2c_Delay()
{
	Delay_10us(1);
}

void I2c_Start()
{
	sda = 1;
	scl = 1;
	I2c_Delay();
	sda = 0;    
	I2c_Delay();
	scl = 0;
}

void I2c_Stop()
{
	scl = 0;
	I2c_Delay(); 
	sda = 0;
	scl = 1;
	I2c_Delay(); 
	sda = 1;    
	I2c_Delay(); 
}

void I2c_Ack()
{
	scl = 0;
	sda = 0;     
	I2c_Delay();
	scl = 1;
	I2c_Delay(); 
	scl = 0;
}

void I2c_No_Ack()
{
	scl = 0;
	sda = 1;     
	I2c_Delay();
	scl = 1;
	I2c_Delay(); 
	scl = 0;
}

unsigned char I2c_Wait_Ack()
{
	unsigned char ack = 0;
	sda = 1;
	scl = 0;	    
	I2c_Delay();
	scl = 1;
	I2c_Delay(); 
	if(sda == 0) 
		ack = 1;
	else 
		ack = 0;
	scl = 0;
	return ack;
}

void I2c_Write_Byte(unsigned char dat)
{
	unsigned char i = 0;
	for(i = 0; i < 8; i++) 
	{
		scl = 0;	
		I2c_Delay();
		if(dat & 0x80) if(dat == 1)  
			sda = 1; 
		else 
			sda = 0; 
		scl = 1; 
		I2c_Delay();		
		dat <<= 1;  
	}
	scl = 0;
}

unsigned char I2c_Read_Byte()
{
	unsigned char dat = 0, i = 0;
	for(i = 0; i < 8; i++) 
	{
		dat <<= 1;  
		scl = 0;	
		I2c_Delay();
		scl = 1;     
		I2c_Delay();
		if(sda)      
			dat |= 0x1;
	}
	scl = 0;
	return dat;
}

void At24c256_Write(unsigned char Paddr,unsigned char Waddr, unsigned char dat)
{
	I2c_Start();
	I2c_Write_Byte(0xA0);
	I2c_Wait_Ack();  
	I2c_Write_Byte(Paddr); 
	I2c_Wait_Ack();	
	I2c_Write_Byte(Waddr); 
	I2c_Wait_Ack();
	I2c_Write_Byte(dat);  
	I2c_Wait_Ack();
	I2c_Stop();
	Delay_10us(1000);    
}

unsigned char At24c256_Read(unsigned char addr)
{
	unsigned char dat = 0, i = 0;
	I2c_Start();
	I2c_Write_Byte(0xA0); 
	I2c_Wait_Ack();       
	I2c_Write_Byte(addr); 
	I2c_Wait_Ack(); 
	I2c_Start();
	I2c_Write_Byte(0xA1); 
	I2c_Wait_Ack();       
	dat = I2c_Read_Byte(); 
	I2c_Wait_Ack(); 
	I2c_Stop();
	return dat;
}

void UART()
{
	SCON=0x50; 
	PCON &= 0X7F; //SMOD=0
	TMOD |= 0X20; //Timer1模式2
	TH1=TL1=0XFD; //鮑率9600
	IE = 0X90;
	TR1 =1;        
}

void UARTInt(void) interrupt 4
{
	if(TI)
	{
		TI=0;//Reset T1
	}
}

int main()
{	
	unsigned char i = 0;
	Delay_10us(100);  
	UART();
	while(1)
	{
		for(i = 0; i < 8; i++)
		{		
			At24c256_Write(0,i,i); //0 page i行數 i資料
			SBUF = coding[At24c256_Read(i)]; //UART	
			Delay_10us(50000);	 	
		}
	}
}
