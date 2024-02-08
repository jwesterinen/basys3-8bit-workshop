#define		F_CPU		12000000UL

#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
#include <stdio.h>

/*****************************************************************************/

#define		__IOR(x)	(*(volatile uint8_t *)(0x20+(x)))
#define		__IOW(x)	(*(volatile uint16_t *)(0x20+(x)))

/*****************************************************************************/


#define		IO_BASE_UART0		0x00
#define		IO_BASE_PORTOUT0	0x04
#define		IO_BASE_TIMER0		0x08

/* uart.h */

#define		UDR0		__IOR(IO_BASE_UART0+0x00)
#define		UCSRA0		__IOR(IO_BASE_UART0+0x01)
#define		UCSRB0		__IOR(IO_BASE_UART0+0x02)
#define		UBRR0		__IOR(IO_BASE_UART0+0x03)

/* UCSRA */
#define		RXB8		0
#define		PE		2
#define		DOR		3
#define		FE		4
#define		UDRE		5
#define		TXC		6
#define		RXC		7

/* UCSRB */
#define		TXB8		0
#define		UCSZ		1
#define		UPM0		2
#define		UPM1		3
#define		USBS		4
#define		UDRIE		5
#define		TXCIE		6
#define		RXCIE		7	

/* timer.h */

#define		TCNT0		__IOW(IO_BASE_TIMER0+0x00)
#define		TCR0		__IOR(IO_BASE_TIMER0+0x02)
#define		TSR0		__IOR(IO_BASE_TIMER0+0x03)

#define		TOF		7	/* timer overflow */
#define		TOFIE		7	/* timer overflow interrupt enable */
#define		TPRESC0		0	/* timer prescaler bit 0 */
#define		TPRESC1		1	/* timer prescaler bit 1 */

/* port.h */

#define		PORTOUT0	__IOR(IO_BASE_PORTOUT0+0x00)

/*****************************************************************************/

static int uart_putchar(char c, FILE *stream)
{
 loop_until_bit_is_set(UCSRA0, UDRE);
 UDR0 = c;
 return(0);
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

/*****************************************************************************/

/* RXC */
ISR(_VECTOR(3))
{
 uint8_t	c;
 c=UDR0;
 if ( 'a' <= c && c <= 'z' )     c-=('a'-'A');
 UDR0=c;
}

ISR(_VECTOR(1))
{
 TCR0=TCR0;
 PORTOUT0 ^= 0x02;
}


static inline void msleep(uint16_t msec)
{
 while ( msec )
  {	_delay_loop_2((uint32_t)F_CPU/4000UL);
	msec--;
  }
}

static void test_memory_buffer(void)
{
 static char *	string="0123456789";
 uint8_t	length=10,offset,cnt;

 msleep(10);

 offset=0;
 cnt=0;
 while ( 1 )
  {	uint8_t	c;

	while ( ! (UCSRA0&(1<<RXC)) );
	c=UDR0;

	if ( c != 10 )
	 {	c=string[offset];
		offset++;
		if ( length==offset )	offset=0;
	 }

	UDR0=c; 
	cnt++;
	PORTOUT0=cnt&3;
  };
 

}

static void test_uppercase(void)
{
 while ( 1 )
  {	uint8_t	c;
	while ( ! (UCSRA0&(1<<RXC)) );
	c=UDR0;
	if ( 'a' <= c && c <= 'z' )	c-=('a'-'A');
	while ( ! (UCSRA0&(1<<UDRE)) );
	UDR0=c; 
  };
 
}

static void test_printf(void)
{
 int	i;

 stdout=&mystdout;

 i=0;
 PORTOUT0 = 0x90;
 PORTOUT0 ^= 0x03;

 while ( 1 )
  {	PORTOUT0 ^= 0x03;
	printf("[x] %d => %d\n",i,i*i);
	msleep(1000);
	i++;
  }
}

void test_interrupt(void)
{
 UCSRB0 |= (1<<RXCIE);
 
 TCR0 = 0x02;
 TCR0 |= (1<<TOFIE);

 sei();
 while ( 1 )
  {	PORTOUT0 ^= 0x01;
	msleep(500);
  }
}

void test_io(void)
{

    uint8_t	c = 0;

    while ( 1 )
    {	
        msleep(500);
        PORTOUT0=c;
        UDR0='a'+c;
        c=(c+1)%4;
    }
}

int main(void)
{

    UBRR0 = 13-1;

    //test_printf();
    //test_interrupt();
    //test_uppercase();
    //test_memory_buffer();
    test_io();

    return(0);
}

