#include <reg51.h>
main()
{
	int i ;
	unsigned char j;
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
	}
}