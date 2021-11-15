#include<reg51.h> 
#define  LED  P3 
void delay (int x); 



main()
{
while(1)
{
	P3=0xfb;
	delay(10000);
	P3=0xfd;
	delay(10000);
	P3=0xfe;
	delay(10000);
			}
}


void delay(int x)
{
	int i;
	for (i=0;i<x;i++);
}