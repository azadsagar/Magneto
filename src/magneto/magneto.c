#define F_CPU 8000000UL
#define and &&
#define or ||

#include<avr/io.h>
#include<util/delay.h>

#include "usart.c"

#define DEBUG

#define modof(t) t>0?t:t*-1

#define frwd 0xfa
#define back 0xf5
#define left 0xf3
#define right 0xfc
#define stop 0xf0

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


void sendPacket(char *packet)
{
	while(*packet)
	{
		usart_putch(*packet);
		packet++;
		_delay_ms(20);
	}
}

int main()
{
	#ifdef DEBUG
	char str[5];
	#endif
	
	
	char packet[]="#@xx@$";
	char speed=255;
	char ch;
	unsigned char direction=0x00;

	unsigned int x,y,xangle=90,yangle=90;//,lxangle=90,lyangle=90;
	//int angle;
	unsigned long xVal=0,yVal=0;
	unsigned int xMin,xMax,yMin,yMax;
	int i;
	
	DDRB=0xff;
	
	initusart();
	initadc();
	
	_delay_ms(6000);
	//DDRD=0xff;
	
	printserial("Ready for callibration press C when ready\r\n");
	do
	{
		ch=usart_getch();
	}while(ch!='C');
	
	for(i=0;i<200;i++)
	{
		x=readadc(2);
		_delay_ms(10);
		y=readadc(1);
		_delay_ms(10);
		
		xVal+=x;
		yVal+=y;
	}
	
	//find avg
	xVal/=200;
	yVal/=200;
	
	xMin=xVal-65;
	xMax=xVal+65;
	yMin=yVal-65;
	yMax=yVal+65;
	
	x=xVal;
	y=yVal;
	
	printserial("xVal=");
	int2str(x,str);
	printserial(str);
	printserial(" xMin=");
	int2str(xMin,str);
	printserial(str);
	printserial(" xMax=");
	int2str(xMax,str);
	printserial(str);
	printserial("\r\n");
	
	printserial("yVal=");
	int2str(y,str);
	printserial(str);
	printserial(" yMin=");
	int2str(yMin,str);
	printserial(str);
	printserial(" yMax=");
	int2str(yMax,str);
	printserial(str);
	printserial("\r\n");
	
	
	
	PORTB=0x00;

	_delay_ms(15000);
	/*do
	{
		ch=usart_getch();
	}while(ch!='C');
	
	usart_putch('$');*/
	
	while(1)
	{

		x=readadc(2);
		_delay_ms(10);
		y=readadc(1);
		_delay_ms(10);
		
		if(x>=xMin and x<=xMax)
		{
			xangle=x-xMin;
			xangle=xangle/0.72;
			x=xangle;
		}
		
		if(y>=yMin and y<=yMax)
		{
			yangle=y-yMin;
			yangle=yangle/0.72;
			y=yangle;
			
		}
		
		if(yangle<70)
		{
			direction=frwd;
		}
		else if(yangle>110)
		{
			direction=back;
		}
		
		if(xangle<70)
		{
			direction=direction & left;
		}
		else if (xangle>110)
		{
			direction=direction & right;
		}
		
		
		if((xangle>=70 and xangle<=110) and (yangle>=70 and yangle<=110))
		{
			direction=stop;
		}
		
		if((yangle>=0 and yangle<=20) or (yangle>=160 and yangle<=180))
		{
			speed=255;
		}
		else if((yangle>=21 and yangle<=40) or (yangle>=140 and yangle<=159))
		{
			speed=200;
		}
		else if((yangle>=41 and yangle<=60) or (yangle>=120 and yangle<=139))
		{
			speed=150;
		}
		else if((yangle>=61 and yangle<=70) or (yangle>=110 and yangle<=119))
		{
			speed=100;
		}
		
		//assemble packet
		packet[2]=direction;
		packet[3]=speed;
		
		/*
		printserial("\r\nX = ");
		int2str(x,str);
		printserial(str);
		printserial(" | Y = ");
		int2str(y,str);
		printserial(str);
		printserial(" | Speed = ");
		int2str(speed,str);
		printserial(str);
		printserial(" | Dir = ");
		int2str(direction,str);
		printserial(str);
		usart_putch('\r');
		usart_putch('\n');*/
		sendPacket(packet);
		//_delay_ms(50);

	}
	return 0;
}
