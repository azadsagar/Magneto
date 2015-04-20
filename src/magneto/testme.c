#define F_CPU 8000000UL
#define and &&
#define or ||

#include<avr/io.h>
#include<util/delay.h>

#include "usart.c"

#define DEBUG

#define modof(t) t>0?t:(t*-1)

#ifdef DEBUG

void int2str(unsigned int num,char *str)
{
	char i=4;
	*(str+i)=0;

	while(num)
	{
		i--;
		*(str+i)=(num%10)+48;
		num/=10;
	}

	while(i)
	{
		i--;
		*(str+i)=48;
	}

}

void printserial(char *str)
{
	while(*str)
	{
		usart_putch(*str);
		str++;
	}
}

#endif

void initadc()
{
	ADMUX=0x00; //internal refrence off
	ADCSRA=0x87;
}

unsigned int readadc(unsigned char ch)//must be b/w 0 and 7
{

	ch= ch & 0b00000111; // channel must be b/w 0 to 7
	ADMUX &=0xf8;
	ADMUX |= ch; // selecting channel

 
	ADCSRA|=(1<<ADSC); // start conversion
	while(!(ADCSRA & (1<<ADIF))); // waiting for ADIF, conversion complete
	ADCSRA|=(1<<ADIF); // clearing of ADIF, it is done by writing 1 to it


	return (ADCW);

}

int main()
{
	#ifdef DEBUG
	char str[5];
	#endif
	
	//char *serialstr[]={"Steady\r\n","Forward\r\n","Back\r\n","Left\r\n","Right\r\n"};

	unsigned int x,y,z,xangle=0,yangle=0,lxangle=90,lyangle=90;
	int angle;
	
	DDRB=0xff;
	

	initusart();
	initadc();
	
	_delay_ms(1000);
	//DDRD=0xff;
	
	PORTB=0x00;

	while(1)
	{

		x=readadc(2);
		_delay_ms(10);
		y=readadc(1);
		_delay_ms(10);
		z=readadc(0);
		_delay_ms(10);
		
			
		#ifdef DEBUG
		

		if(x>=272 and x<=402)
		{
			xangle=x-272;
			xangle=xangle/0.72;
			x=xangle;
		}
		
		if(y>=265 and y<=395)
		{
			yangle=y-265;
			yangle=yangle/0.72;
			y=yangle;
		}
		
		angle=lxangle-xangle;
		
		if((modof(angle))>=20)
		{
			printserial("X = ");
			int2str(x,str);
			printserial(str);
			usart_putch('\r');
			usart_putch('\n');
			lxangle=xangle;
		}
		
		angle=lyangle-yangle;
		
		if((modof(angle))>=20)
		{
			printserial("Y = ");
			int2str(y,str);
			printserial(str);
			usart_putch('\r');
			usart_putch('\n');
			lyangle=yangle;
		}
		
		
		//_delay_ms(2000);
		
		#endif

	}
	return 0;
}
