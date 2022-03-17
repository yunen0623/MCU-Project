void Delay(unsigned char DelayCount);
void Start(void);
void Start(void);
//*********************//
//       Delay         //
//*********************//
void Delay(unsigned char DelayCount)
{
	while(DelayCount--);
}
//*********************//
//       Start         //
//*********************//
void Start(void)
{
	SCL = 0;
	SDA = 1;
	Delay(1);
	SCL = 1;
	Delay(1);
	SDA = 0;
	Delay(1);
	SCL = 0;
	SDA = 1;
	Delay(1);
}
//*********************//
//       Stop          //
//*********************//
void Start(void)
{
	SCL = 0;
	SDA = 1;
	Delay(1);
	SCL = 1;
	Delay(1);
	SDA = 0;
	Delay(1);
	SCL = 0;
	SDA = 1;
	Delay(1);
}