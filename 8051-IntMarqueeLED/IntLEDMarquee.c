#include <reg51.h>
#define LED P1
unsigned LED_1=0xFE;
char a; 

main()
{
	IT1 = 1;
	IE = 0x84;
	{
		int i;
		unsigned j;
		a=1;
	while(1)
		{
				if(a%2==0) //初始為1，雙數
				{
					for(j=0;j<5;j++)
						{
							for(i=0;i<15000;i++);
							P1=(P1<<1)|0x01;
						}
					for(j=0;j<5;j++)
						{
							for(i=0;i<30000;i++);
							P1=(P1>>1)|0x80;
						}
					{
						if(a%2!=0) //初始為1，單數
							{
								P1=0XFE;
							}
					}
				}
		}
	}
}

void INT1Int(void)interrupt 2
{
	a=!a;
}