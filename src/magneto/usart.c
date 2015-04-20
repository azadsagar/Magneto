#ifndef USEUSART
#define USEUSART

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

inline void usart_putch(unsigned char send)
{
	while((UCSRA & (1 << UDRE))==0); // Do nothing until UDR is ready..
	// for more data to be written to it
	UDR = send; // Send the byte 
}
 
inline unsigned char usart_getch()
{
	while ((UCSRA & (1 << RXC)) == 0);
	// Do nothing until data have been received and is ready to be read from UDR
	return(UDR); // return the byte
}

inline void initusart()
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