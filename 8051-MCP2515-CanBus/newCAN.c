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
bit busy = 1; //UART 發送數據狀態
unsigned char UART1_RX_Buffer[UART1_Rx_Buff_LEN]; //建立接收陣列 大小為100
unsigned char Uart1_Delay = 0 ; //延遲時間，直到數據接收完成
unsigned char Uart1_Write_Count = 0; //UART寫入Buffer Pointer
unsigned char Uart1_Read_Count = 0; //UART讀取Buffer Pointer
unsigned char Uart1_Finish = 0; //接收完當前數據旗標
unsigned char CAN_Flag = 0; //CAN接收到數據旗標
unsigned char CAN_RX_Buffer[8]; //CAN接收數據保存陣列 大小為8byte

void Delay_Nms(unsigned int x)
{
	unsigned int y;

	for (;x>0;x--)
		for (y=0;y<100;y++);
}

unsigned char SPI_ReadByte(void)  //透過SPI讀取一個Byte的資料,rBtye為返回值(讀到的一個DataByte)
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

void SPI_SendByte(unsigned char dt)  //透過SPI發送一個Byte的資料 dt為等待發送的數據
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


//透過SPI向MCP2515指定的地址寫一個byte的資料， dat為準備寫入的資料
void MCP2515_WriteByte(unsigned char addr,unsigned char dat)
{
	CS=0;				
	SPI_SendByte(CAN_WRITE);	//發送寫命令
	SPI_SendByte(addr);			//發送地址
	SPI_SendByte(dat);			//寫入數據
	CS=1;				
}


unsigned char MCP2515_ReadByte(unsigned char addr) //透過SPI從MCP2515指定的地址 讀一個Byte的資料
{
	unsigned char rByte;
	
	CS=0;		
	SPI_SendByte(CAN_READ);		//發送讀命令
	SPI_SendByte(addr);			//發送地址
	rByte=SPI_ReadByte();		//讀取數據
	CS=1;				
	return rByte;				//返回讀到的一個字節數據
}

//發送reset指令 透過軟體reset MCP2515，將MCP2515設為配置模式
void MCP2515_Reset(void)
{
	CS=0;				
	SPI_SendByte(CAN_RESET);	//發送Reset命令
	CS=1;				
}


//MCP2515初始化--Reset/鮑率設定/暫存器設定
void MCP2515_Init(void)
{
	unsigned char temp=0;

	MCP2515_Reset();	
	Delay_Nms(1);		
	
	//設置波特率為125Kbps
	//set CNF1,SJW=00,長度為1TQ,BRP=49,TQ=[2*(BRP+1)]/Fsoc=2*50/8M=12.5us
	MCP2515_WriteByte(CNF1,CAN_125Kbps);
	//set CNF2,SAM=0,在採樣點對總線進行一次採樣，PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	MCP2515_WriteByte(CNF2,0x80|PHSEG1_3TQ|PRSEG_1TQ);
	//set CNF3,PHSEG2=(2+1)TQ=3TQ,同時當CANCTRL.CLKEN=1時設定CLKOUT引腳為時間輸出使能位
	MCP2515_WriteByte(CNF3,PHSEG2_3TQ);
	
	MCP2515_WriteByte(TXB0SIDH,0xFF);//發送緩衝器0標準標識符高位
	MCP2515_WriteByte(TXB0SIDL,0xEB);//發送緩衝器0標準標識符低位(第3位為發送拓展標識符使能位)
	MCP2515_WriteByte(TXB0EID8,0xFF);//發送緩衝器0拓展標識符高位
	MCP2515_WriteByte(TXB0EID0,0xFF);//發送緩衝器0拓展標識符低位
	
	MCP2515_WriteByte(RXB0SIDH,0x00);//清空接收緩衝器0的標準標識符高位
	MCP2515_WriteByte(RXB0SIDL,0x00);//清空接收緩衝器0的標準標識符低位
	MCP2515_WriteByte(RXB0EID8,0x00);//清空接收緩衝器0的拓展標識符高位
	MCP2515_WriteByte(RXB0EID0,0x00);//清空接收緩衝器0的拓展標識符低位
	MCP2515_WriteByte(RXB0CTRL,0x40);//僅僅接收拓展標識符的有效信息
	MCP2515_WriteByte(RXB0DLC,DLC_8);//設置接收數據的長度為8個字節
	
	MCP2515_WriteByte(RXF0SIDH,0xFF);//配置驗收濾波寄存器n標準標識符高位
	MCP2515_WriteByte(RXF0SIDL,0xEB);//配置驗收濾波寄存器n標準標識符低位(第3位為接收拓展標識符使能位)
	MCP2515_WriteByte(RXF0EID8,0xFF);//配置驗收濾波寄存器n拓展標識符高位
	MCP2515_WriteByte(RXF0EID0,0xFF);//配置驗收濾波寄存器n拓展標識符低位

	MCP2515_WriteByte(RXM0SIDH,0x00);//配置驗收屏蔽寄存器n標準標識符高位
	MCP2515_WriteByte(RXM0SIDL,0x00);//配置驗收屏蔽寄存器n標準標識符低位
	MCP2515_WriteByte(RXM0EID8,0x00);//配置驗收濾波寄存器n拓展標識符高位
	MCP2515_WriteByte(RXM0EID0,0x00);//配置驗收濾波寄存器n拓展標識符低位
	
	MCP2515_WriteByte(CANINTF,0x00);//清空CAN中斷標誌寄存器的所有位(必須由MCU清空)
	MCP2515_WriteByte(CANINTE,0x01);//配置CAN中斷使能寄存器的接收緩衝器0滿中斷使能,其它位禁止中斷
	
	MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//將MCP2515設置為正常模式,退出配置模式
	
	temp=MCP2515_ReadByte(CANSTAT);//讀取CAN狀態寄存器的值
	if(OPMODE_NORMAL!=(temp&&0xE0))//判斷MCP2515是否已經進入正常模式
	{
		MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//再次將MCP2515設置為正常模式,退出配置模式
	}
}


//CAN發送指定長度的資料  *CAN_TX_Buf(等待發送的Buffer pointer) len(等待發送的數據長度)
void CAN_Send_Buffer(unsigned char *CAN_TX_Buf,unsigned char len)
{
	unsigned char j,dly,count;

	count=0;
	while(count<len)
	{
		dly=0;
		while((MCP2515_ReadByte(TXB0CTRL)&0x08) && (dly<50))//快速讀某些狀態指令,等待TXREQ標誌清零 page15
		{
			Delay_Nms(1);
			dly++;
		}
				
		for(j=0;j<8;)
		{
			MCP2515_WriteByte(TXB0D0+j,CAN_TX_Buf[count++]);//將待發送的數據寫入發送緩衝寄存器
			j++;
			if(count>=len) 
				break;
		}
		MCP2515_WriteByte(TXB0DLC,j);//將本幀待發送的數據長度寫入發送緩衝器0的發送長度寄存器
		CS=0;
		MCP2515_WriteByte(TXB0CTRL,0x08);//請求發送報文
		CS=1;
	}
}

//CAN接收資料 *CAN_RX_Buf 等待接收的緩衝區指針,返回len(接收到數據的長度)
unsigned char CAN_Receive_Buffer(unsigned char *CAN_RX_Buf)
{
	unsigned char i=0,len=0,temp=0;

	temp = MCP2515_ReadByte(CANINTF); //CANINTF為中斷標誌暫存器 
	if(temp & 0x01)  //if 結果不為0 , temp = 0x01
	{
		len=MCP2515_ReadByte(RXB0DLC);//讀取接收緩衝器0接收到的數據長度(0~8個字節)
		while(i<len)
		{	
			CAN_RX_Buf[i]=MCP2515_ReadByte(RXB0D0+i);//把CAN接收到的數據放入指定緩衝區
			i++;
		}
	}
	MCP2515_WriteByte(CANINTF,0);//清除中斷標誌位(中斷標誌寄存器必須由MCU清零)
	return len;
}

//Timer0_Init 初始化Timer0
void Timer0_Init(void)
{
	TMOD=0x21; //Timer0 gate c/t mode1  
	TH0=(65536-46080)/256;//初始化計時值
	TL0=(65536-4608)%256; //0.05S
	TR0=1; //開始計時
	ET0 = 1; //開啟Timer0中斷
	EA = 1; //開始所有可致能中斷	
}

//Timer0_ISR ,檢查UART是否接收數據完成
void Timer0_ISR(void) interrupt 1
{
	if(Uart1_Delay>0)
	{
		Uart1_Delay--;
		if(Uart1_Delay==0)
		{
			//延時時間到再沒有接收到新的串口數據，表示1幀數據接收完成
			if(Uart1_Write_Count != Uart1_Read_Count) 
				Uart1_Finish=1;
		}
	}

	TH0=(65536-46080)/256;//初始化計時值
	TL0=(65536-4608)%256; //0.05S
}

//外部中斷初始化
void Exint1_Init(void)
{
    PX1=1;		//設置外部中斷1的中斷優先級為高優先級
    IT1 = 1;	//設置INT1的中斷類型 (1:僅下降沿 0:上升沿和下降沿)
    EX1 = 1;	//使能INT1中斷
    EA = 1; 	//使能總中斷
}

//外部中斷1服務函數
void Exint1_ISR(void) interrupt 2
{
	CAN_Flag=1;//CAN接收到數據標誌
}

//UART初始化設定
void UART1_Init_Config(void)	
{
	SCON=0x50; //串列模式1
	PCON&=0x7F; //SMOD = 0
	TMOD |= 0x21; //Timer1模式2
	TH1=TL1=0xFD; // 9600
	TR1 = 1; //Timer1啟動
	ES = 1;//UART啟動
	EA = 1; //開始所有致能中斷
}

//UART 指針,讀寫指針各+1  *pnt(指向UART中讀與寫的緩衝區指針)
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
	//接收數據
	if(RI)
	{
		RI = 0;//清除RI位
		ch=SBUF;
		UART1_RX_Buffer[Uart1_Write_Count]=ch;	//將接收到的數據寫入緩衝區
		UART1_Buffer_PntAdd(&Uart1_Write_Count);//寫串口1緩衝區指針加1			
		if(Uart1_Write_Count == Uart1_Read_Count)//如果讀、寫緩衝區指針重疊,則讀指針加1,這時將丟失1個字節數據
		{
			UART1_Buffer_PntAdd(&Uart1_Read_Count);//讀串口1緩衝區指針加1
		}
		Uart1_Delay = 20;//串口1接收數據幀延時(ms)，延時時間到當1幀數據接收完成
	}
	//發送數據
	if (TI)			
	{
		TI = 0;		//清除TI位
		busy = 0;	//清忙標誌(1忙,0空閒)
	}
}

//UART發送一個byte,dat=等待發送的資料
void UART1_SendData(unsigned char dat)
{
    while (busy);	//等待前面的數據發送完成
    busy = 1;		//串口1發送數據忙標誌位(1忙,0空閒)
    SBUF = dat;		//寫數據到UART數據寄存器
}

//UART1發送一個緩衝區數據,*buff：待發送緩衝區首地址,len：待發送數據長度
void UART1_SendBuffer(unsigned char *buff,unsigned int len)
{
	unsigned int i=0;

	if(len<=0) return;

	do
	{
		UART1_SendData(buff[i++]);//發送當前字符
	}
	while(i<len);
}

//CAN發送UART接收到的資料(PC->UART->8051->CAN)
void CAN_Send_Dispose(void)
{
	unsigned char i=0,len=0,write=0,buff[8];
	
	write = Uart1_Write_Count;
	if(Uart1_Write_Count<Uart1_Read_Count) 
		write+=UART1_Rx_Buff_LEN;
	
	if((write-Uart1_Read_Count) >= 8)//如果串口1接收緩衝區中未讀數據大於8個字節,則通過CAN總線發送8個字節數據(CAN發送一幀報文最大8個字節)
	{			
		len = 8;							
	}
	else if(Uart1_Finish == 1)//如果串口1接收緩衝區中未讀數據小於8個字節,且再也沒接收到串口的數據,則CAN發送剩餘的數據
	{
		len = write-Uart1_Read_Count;
		Uart1_Finish=0;//當前一幀數據接收完成標誌			
	}
	else return;//如果串口1接收緩衝區中未讀數據小於8個字節,且還在接收串口的數據則CAN先不發數據,等夠8個字節了再發
	
	for(i=0;i<len;i++)
	{
		buff[i] = UART1_RX_Buffer[Uart1_Read_Count];//將串口接收緩衝區的數據複製到CAN發送臨時緩衝區buff
		UART1_Buffer_PntAdd(&Uart1_Read_Count);//讀串口1緩衝區指針加1
	}	
	CAN_Send_Buffer(buff,len);//CAN發送指定長度的數據
}

void main()
{
	Timer0_Init(); 
	UART1_Init_Config();
	Exint1_Init();
	MCP2515_Init();
	
	while(1)
	{
		if(Uart1_Write_Count != Uart1_Read_Count)//如果讀指針不等寫指針,則證明串口1接收到數據
		{
			CAN_Send_Dispose();//CAN發送UART1接收到的數據
		}
		else if(Uart1_Finish == 1)
		{
			Uart1_Finish = 0;
		}

		while((CAN_Flag == 1) || ((P3&0x08) == 0))  
			//P3=INT1=P3^3 00001000&00001000=0x08 00000000&00001000=0,CAN_Flag =1 或 P3無中斷時
		{
			unsigned char len;
			CAN_Flag=0;//CAN接收到數據標誌
			len = CAN_Receive_Buffer(CAN_RX_Buffer);
			
			if(len != 0)
				UART1_SendBuffer(CAN_RX_Buffer,len);//UART1發送一個緩衝區數據
		}
	}
}







