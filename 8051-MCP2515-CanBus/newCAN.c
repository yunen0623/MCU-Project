#include <reg51.h>
#include "MCP2515.H"
#include <intrins.h>
sbit SCK=P1^7;
sbit MOSI=P1^5; 
sbit MISO=P1^6;
sbit CS=P1^4;
#define	CAN_10Kbps	0x31
#define CAN_25Kbps	0x13
#define CAN_50Kbps	0x09
#define CAN_100Kbps	0x04
#define CAN_125Kbps	0x03
#define CAN_250Kbps	0x01
#define	CAN_500Kbps	0x00
#define UART1_Rx_Buff_LEN 100 //UART Receive Databuffer len
bit busy = 1; //UART �o�e�ƾڪ��A
unsigned char UART1_RX_Buffer[UART1_Rx_Buff_LEN]; //�إ߱����}�C �j�p��100
unsigned char Uart1_Delay = 0 ; //����ɶ��A����ƾڱ�������
unsigned char Uart1_Write_Count = 0; //UART�g�JBuffer Pointer
unsigned char Uart1_Read_Count = 0; //UARTŪ��Buffer Pointer
unsigned char Uart1_Finish = 0; //��������e�ƾںX��
unsigned char CAN_Flag = 0; //CAN������ƾںX��
unsigned char CAN_RX_Buffer[8]; //CAN�����ƾګO�s�}�C �j�p��8byte

void Delay_Nms(unsigned int x)
{
	unsigned int y;

	for (;x>0;x--)
		for (y=0;y<100;y++);
}

unsigned char SPI_ReadByte(void)  //�z�LSPIŪ���@��Byte�����,rBtye����^��(Ū�쪺�@��DataByte)
{
	unsigned char i,rByte=0;
	
	SCK=0;
	for(i=0;i<8;i++)
	{
		SCK=1;
		rByte<<=1;
		rByte|=MISO;
		SCK=0;	
	}
	return rByte;
}

void SPI_SendByte(unsigned char dt)  //�z�LSPI�o�e�@��Byte����� dt�����ݵo�e���ƾ�
{
	unsigned char i;
		
	for(i=0;i<8;i++)
	{	
		SCK=0;
		if((dt<<i)&0x80)
			MOSI=1;
		else
			MOSI=0;					
		SCK=1;
	}
	SCK=0;
}


//�z�LSPI�VMCP2515���w���a�}�g�@��byte����ơA dat���ǳƼg�J�����
void MCP2515_WriteByte(unsigned char addr,unsigned char dat)
{
	CS=0;				
	SPI_SendByte(CAN_WRITE);	//�o�e�g�R�O
	SPI_SendByte(addr);			//�o�e�a�}
	SPI_SendByte(dat);			//�g�J�ƾ�
	CS=1;				
}


unsigned char MCP2515_ReadByte(unsigned char addr) //�z�LSPI�qMCP2515���w���a�} Ū�@��Byte�����
{
	unsigned char rByte;
	
	CS=0;		
	SPI_SendByte(CAN_READ);		//�o�eŪ�R�O
	SPI_SendByte(addr);			//�o�e�a�}
	rByte=SPI_ReadByte();		//Ū���ƾ�
	CS=1;				
	return rByte;				//��^Ū�쪺�@�Ӧr�`�ƾ�
}

//�o�ereset���O �z�L�n��reset MCP2515�A�NMCP2515�]���t�m�Ҧ�
void MCP2515_Reset(void)
{
	CS=0;				
	SPI_SendByte(CAN_RESET);	//�o�eReset�R�O
	CS=1;				
}


//MCP2515��l��--Reset/�j�v�]�w/�Ȧs���]�w
void MCP2515_Init(void)
{
	unsigned char temp=0;

	MCP2515_Reset();	
	Delay_Nms(1);		
	
	//�]�m�i�S�v��125Kbps
	//set CNF1,SJW=00,���׬�1TQ,BRP=49,TQ=[2*(BRP+1)]/Fsoc=2*50/8M=12.5us
	MCP2515_WriteByte(CNF1,CAN_125Kbps);
	//set CNF2,SAM=0,�b�ļ��I���`�u�i��@���ļˡAPHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	MCP2515_WriteByte(CNF2,0x80|PHSEG1_3TQ|PRSEG_1TQ);
	//set CNF3,PHSEG2=(2+1)TQ=3TQ,�P�ɷ�CANCTRL.CLKEN=1�ɳ]�wCLKOUT�޸}���ɶ���X�ϯ��
	MCP2515_WriteByte(CNF3,PHSEG2_3TQ);
	
	MCP2515_WriteByte(TXB0SIDH,0xFF);//�o�e�w�ľ�0�зǼ��ѲŰ���
	MCP2515_WriteByte(TXB0SIDL,0xEB);//�o�e�w�ľ�0�зǼ��ѲŧC��(��3�쬰�o�e�ݮi���ѲŨϯ��)
	MCP2515_WriteByte(TXB0EID8,0xFF);//�o�e�w�ľ�0�ݮi���ѲŰ���
	MCP2515_WriteByte(TXB0EID0,0xFF);//�o�e�w�ľ�0�ݮi���ѲŧC��
	
	MCP2515_WriteByte(RXB0SIDH,0x00);//�M�ű����w�ľ�0���зǼ��ѲŰ���
	MCP2515_WriteByte(RXB0SIDL,0x00);//�M�ű����w�ľ�0���зǼ��ѲŧC��
	MCP2515_WriteByte(RXB0EID8,0x00);//�M�ű����w�ľ�0���ݮi���ѲŰ���
	MCP2515_WriteByte(RXB0EID0,0x00);//�M�ű����w�ľ�0���ݮi���ѲŧC��
	MCP2515_WriteByte(RXB0CTRL,0x40);//�ȶȱ����ݮi���ѲŪ����īH��
	MCP2515_WriteByte(RXB0DLC,DLC_8);//�]�m�����ƾڪ����׬�8�Ӧr�`
	
	MCP2515_WriteByte(RXF0SIDH,0xFF);//�t�m�禬�o�i�H�s��n�зǼ��ѲŰ���
	MCP2515_WriteByte(RXF0SIDL,0xEB);//�t�m�禬�o�i�H�s��n�зǼ��ѲŧC��(��3�쬰�����ݮi���ѲŨϯ��)
	MCP2515_WriteByte(RXF0EID8,0xFF);//�t�m�禬�o�i�H�s��n�ݮi���ѲŰ���
	MCP2515_WriteByte(RXF0EID0,0xFF);//�t�m�禬�o�i�H�s��n�ݮi���ѲŧC��

	MCP2515_WriteByte(RXM0SIDH,0x00);//�t�m�禬�̽��H�s��n�зǼ��ѲŰ���
	MCP2515_WriteByte(RXM0SIDL,0x00);//�t�m�禬�̽��H�s��n�зǼ��ѲŧC��
	MCP2515_WriteByte(RXM0EID8,0x00);//�t�m�禬�o�i�H�s��n�ݮi���ѲŰ���
	MCP2515_WriteByte(RXM0EID0,0x00);//�t�m�禬�o�i�H�s��n�ݮi���ѲŧC��
	
	MCP2515_WriteByte(CANINTF,0x00);//�M��CAN���_�лx�H�s�����Ҧ���(������MCU�M��)
	MCP2515_WriteByte(CANINTE,0x01);//�t�mCAN���_�ϯ�H�s���������w�ľ�0�����_�ϯ�,�䥦��T��_
	
	MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//�NMCP2515�]�m�����`�Ҧ�,�h�X�t�m�Ҧ�
	
	temp=MCP2515_ReadByte(CANSTAT);//Ū��CAN���A�H�s������
	if(OPMODE_NORMAL!=(temp&&0xE0))//�P�_MCP2515�O�_�w�g�i�J���`�Ҧ�
	{
		MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//�A���NMCP2515�]�m�����`�Ҧ�,�h�X�t�m�Ҧ�
	}
}


//CAN�o�e���w���ת����  *CAN_TX_Buf(���ݵo�e��Buffer pointer) len(���ݵo�e���ƾڪ���)
void CAN_Send_Buffer(unsigned char *CAN_TX_Buf,unsigned char len)
{
	unsigned char j,dly,count;

	count=0;
	while(count<len)
	{
		dly=0;
		while((MCP2515_ReadByte(TXB0CTRL)&0x08) && (dly<50))//�ֳtŪ�Y�Ǫ��A���O,����TXREQ�лx�M�s page15
		{
			Delay_Nms(1);
			dly++;
		}
				
		for(j=0;j<8;)
		{
			MCP2515_WriteByte(TXB0D0+j,CAN_TX_Buf[count++]);//�N�ݵo�e���ƾڼg�J�o�e�w�ıH�s��
			j++;
			if(count>=len) 
				break;
		}
		MCP2515_WriteByte(TXB0DLC,j);//�N���V�ݵo�e���ƾڪ��׼g�J�o�e�w�ľ�0���o�e���ױH�s��
		CS=0;
		MCP2515_WriteByte(TXB0CTRL,0x08);//�ШD�o�e����
		CS=1;
	}
}

//CAN������� *CAN_RX_Buf ���ݱ������w�İϫ��w,��^len(������ƾڪ�����)
unsigned char CAN_Receive_Buffer(unsigned char *CAN_RX_Buf)
{
	unsigned char i=0,len=0,temp=0;

	temp = MCP2515_ReadByte(CANINTF); //CANINTF�����_�лx�Ȧs�� 
	if(temp & 0x01)  //if ���G����0 , temp = 0x01
	{
		len=MCP2515_ReadByte(RXB0DLC);//Ū�������w�ľ�0�����쪺�ƾڪ���(0~8�Ӧr�`)
		while(i<len)
		{	
			CAN_RX_Buf[i]=MCP2515_ReadByte(RXB0D0+i);//��CAN�����쪺�ƾک�J���w�w�İ�
			i++;
		}
	}
	MCP2515_WriteByte(CANINTF,0);//�M�����_�лx��(���_�лx�H�s��������MCU�M�s)
	return len;
}

//Timer0_Init ��l��Timer0
void Timer0_Init(void)
{
	TMOD=0x21; //Timer0 gate c/t mode1  
	TH0=(65536-46080)/256;//��l�ƭp�ɭ�
	TL0=(65536-4608)%256; //0.05S
	TR0=1; //�}�l�p��
	ET0 = 1; //�}��Timer0���_
	EA = 1; //�}�l�Ҧ��i�P�त�_	
}

//Timer0_ISR ,�ˬdUART�O�_�����ƾڧ���
void Timer0_ISR(void) interrupt 1
{
	if(Uart1_Delay>0)
	{
		Uart1_Delay--;
		if(Uart1_Delay==0)
		{
			//���ɮɶ���A�S��������s����f�ƾڡA���1�V�ƾڱ�������
			if(Uart1_Write_Count != Uart1_Read_Count) 
				Uart1_Finish=1;
		}
	}

	TH0=(65536-46080)/256;//��l�ƭp�ɭ�
	TL0=(65536-4608)%256; //0.05S
}

//�~�����_��l��
void Exint1_Init(void)
{
    PX1=1;		//�]�m�~�����_1�����_�u���Ŭ����u����
    IT1 = 1;	//�]�mINT1�����_���� (1:�ȤU���u 0:�W�ɪu�M�U���u)
    EX1 = 1;	//�ϯ�INT1���_
    EA = 1; 	//�ϯ��`���_
}

//�~�����_1�A�Ȩ��
void Exint1_ISR(void) interrupt 2
{
	CAN_Flag=1;//CAN������ƾڼлx
}

//UART��l�Ƴ]�w
void UART1_Init_Config(void)	
{
	SCON=0x50; //��C�Ҧ�1
	PCON&=0x7F; //SMOD = 0
	TMOD |= 0x21; //Timer1�Ҧ�2
	TH1=TL1=0xFD; // 9600
	TR1 = 1; //Timer1�Ұ�
	ES = 1;//UART�Ұ�
	EA = 1; //�}�l�Ҧ��P�त�_
}

//UART ���w,Ū�g���w�U+1  *pnt(���VUART��Ū�P�g���w�İϫ��w)
void UART1_Buffer_PntAdd(unsigned char *pnt)
{
	*pnt+=1;
	if(*pnt >= UART1_Rx_Buff_LEN) 
		*pnt=0;
}

//UART_ISR 
void UART1_ISR(void) interrupt 4
{
	unsigned char ch;
	//�����ƾ�
	if(RI)
	{
		RI = 0;//�M��RI��
		ch=SBUF;
		UART1_RX_Buffer[Uart1_Write_Count]=ch;	//�N�����쪺�ƾڼg�J�w�İ�
		UART1_Buffer_PntAdd(&Uart1_Write_Count);//�g��f1�w�İϫ��w�[1			
		if(Uart1_Write_Count == Uart1_Read_Count)//�p�GŪ�B�g�w�İϫ��w���|,�hŪ���w�[1,�o�ɱN�ᥢ1�Ӧr�`�ƾ�
		{
			UART1_Buffer_PntAdd(&Uart1_Read_Count);//Ū��f1�w�İϫ��w�[1
		}
		Uart1_Delay = 20;//��f1�����ƾڴV����(ms)�A���ɮɶ����1�V�ƾڱ�������
	}
	//�o�e�ƾ�
	if (TI)			
	{
		TI = 0;		//�M��TI��
		busy = 0;	//�M���лx(1��,0�Ŷ�)
	}
}

//UART�o�e�@��byte,dat=���ݵo�e�����
void UART1_SendData(unsigned char dat)
{
    while (busy);	//���ݫe�����ƾڵo�e����
    busy = 1;		//��f1�o�e�ƾڦ��лx��(1��,0�Ŷ�)
    SBUF = dat;		//�g�ƾڨ�UART�ƾڱH�s��
}

//UART1�o�e�@�ӽw�İϼƾ�,*buff�G�ݵo�e�w�İϭ��a�},len�G�ݵo�e�ƾڪ���
void UART1_SendBuffer(unsigned char *buff,unsigned int len)
{
	unsigned int i=0;

	if(len<=0) return;

	do
	{
		UART1_SendData(buff[i++]);//�o�e��e�r��
	}
	while(i<len);
}

//CAN�o�eUART�����쪺���(PC->UART->8051->CAN)
void CAN_Send_Dispose(void)
{
	unsigned char i=0,len=0,write=0,buff[8];
	
	write = Uart1_Write_Count;
	if(Uart1_Write_Count<Uart1_Read_Count) 
		write+=UART1_Rx_Buff_LEN;
	
	if((write-Uart1_Read_Count) >= 8)//�p�G��f1�����w�İϤ���Ū�ƾڤj��8�Ӧr�`,�h�q�LCAN�`�u�o�e8�Ӧr�`�ƾ�(CAN�o�e�@�V����̤j8�Ӧr�`)
	{			
		len = 8;							
	}
	else if(Uart1_Finish == 1)//�p�G��f1�����w�İϤ���Ū�ƾڤp��8�Ӧr�`,�B�A�]�S�������f���ƾ�,�hCAN�o�e�Ѿl���ƾ�
	{
		len = write-Uart1_Read_Count;
		Uart1_Finish=0;//��e�@�V�ƾڱ��������лx			
	}
	else return;//�p�G��f1�����w�İϤ���Ū�ƾڤp��8�Ӧr�`,�B�٦b������f���ƾګhCAN�����o�ƾ�,����8�Ӧr�`�F�A�o
	
	for(i=0;i<len;i++)
	{
		buff[i] = UART1_RX_Buffer[Uart1_Read_Count];//�N��f�����w�İϪ��ƾڽƻs��CAN�o�e�{�ɽw�İ�buff
		UART1_Buffer_PntAdd(&Uart1_Read_Count);//Ū��f1�w�İϫ��w�[1
	}	
	CAN_Send_Buffer(buff,len);//CAN�o�e���w���ת��ƾ�
}

void main()
{
	Timer0_Init(); 
	UART1_Init_Config();
	Exint1_Init();
	MCP2515_Init();
	
	while(1)
	{
		if(Uart1_Write_Count != Uart1_Read_Count)//�p�GŪ���w�����g���w,�h�ҩ���f1������ƾ�
		{
			CAN_Send_Dispose();//CAN�o�eUART1�����쪺�ƾ�
		}
		else if(Uart1_Finish == 1)
		{
			Uart1_Finish = 0;
		}

		while((CAN_Flag == 1) || ((P3&0x08) == 0))  
			//P3=INT1=P3^3 00001000&00001000=0x08 00000000&00001000=0,CAN_Flag =1 �� P3�L���_��
		{
			unsigned char len;
			CAN_Flag=0;//CAN������ƾڼлx
			len = CAN_Receive_Buffer(CAN_RX_Buffer);
			
			if(len != 0)
				UART1_SendBuffer(CAN_RX_Buffer,len);//UART1�o�e�@�ӽw�İϼƾ�
		}
	}
}







