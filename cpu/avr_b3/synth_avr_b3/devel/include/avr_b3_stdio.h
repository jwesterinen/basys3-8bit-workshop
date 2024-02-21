
// setup for printf
static int uart_putchar(char c, FILE *stream)
{
    loop_until_bit_is_set(UCSRA0, UDRE);
    UDR0 = c;
    
    return(0);
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);


