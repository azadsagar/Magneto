#define F_CPU 8000000UL
 
//usart settings
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

//direction lookup preprocessor directive
#define frwd 0x14
#define back 0x0a
#define left 0x04
#define right 0x10
#define stop 0x00

//pwm pins
#define m1speed OCR1A
#define m2speed OCR1B

//l293d port
#define bot PORTC

// the avr header files
#include<avr/io.h>
#include<util/delay.h>

#define USEPWM
#define USEUSART

#ifdef USEPWM
void initpwm()
{

	TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);		 	//NON Inverted PWM
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10);	//PRESCALER=64 MODE 14(FAST PWM)

	ICR1=255;	//fPWM=50Hz (Period = 20ms Standard).

}
#endif

#ifdef USEUSART
void usart_putch(unsigned char send)
{
	while((UCSRA & (1 << UDRE))==0); // Do nothing until UDR is ready..
	// for more data to be written to it
	UDR = send; // Send the byte 
}
 
unsigned char usart_getch()
{
	while ((UCSRA & (1 << RXC)) == 0);
	// Do nothing until data have been received and is ready to be read from UDR
	return(UDR); // return the byte
}

void init_usart()
{
	UCSRB |= (1 << RXEN) | (1 << TXEN);   
	// Turn on the transmission and reception circuitry
	UCSRC |= (1 << URSEL) | (1<<USBS) | (1 << UCSZ0) | (1 << UCSZ1); 
	// Use 8-bit character sizes
	 
	UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value..
	// into the low byte of the UBRR register
	UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value..
	// into the high byte of the UBRR register
}
#endif

int main()
{
	DDRC=0xff; //set port c as digital output port
	DDRB=0xff; //also speed control pin as output port
	unsigned char ch,signal,tmpSpeed,direction=0x00;
	unsigned int speed;

	bot=stop;
	init_usart();
	_delay_ms(1000);
	initpwm();
	_delay_ms(1000);

	while(1)
	{
		//xbee packet is xpected in following format
		//#@[control signal][speed]@$
		//where control signalis 8 bit
		//and speed value is 8 bit
		//total packet size is 6 bytes
		ch=usart_getch();

		if(ch=='#')							//1st byte #
		{
			ch=usart_getch();
			if(ch=='@')						//2nd byte @
			{
				signal=usart_getch();		//3rd byte
				tmpSpeed=usart_getch();		//4th byte
				ch=usart_getch();
				if(ch=='@')					//5th byte @
				{
					ch=usart_getch();
					if(ch=='$')				//6th byte $
					{
						//packet is valid
						//apply changes
						
						direction=signal & 0x0f;
						direction=direction << 1;
						bot=direction;
						
						//set the speed values
						speed=tmpSpeed;
						//keeping speed same for both the motors
						m1speed=speed;
						m2speed=speed;
					} //discard packet if invalid
				}
			}
		}
		
	}

	return 0;

}
