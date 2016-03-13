/**
 *	USART.h
 *
 *	funções relacionadas à porta serial
 **/

/**
 *	USART_config
 *
 *	configura valores iniciais dos registradores
 *	envolvidos na transmissão serial
 *
 *	@register TXSTA = 00000100
 *	@bit	  BRGH  = 1		: high speed baud rate 
 *
 *	@register SPBRG = 129		: para um baudrate de 9600 (seguindo tabela do datasheet)
 *
 *	@register RCSTA = 00000000	: <!> ver se precisa de alguma modificação
 *
 *	@register TRISC = 11111111	: todos os pinos C como saída (RC6,RC7 setados, como explicado no datasheet)
 **/

void USART_config()
{
        TXSTA  = 0b00000100;  
        SPBRG  = 129;        
        RCSTA  = 0b00000000;
        TRISC  = 0b11111111;
}

/**
 *	USART_send_char
 *
 *	envia caractere através da porta serial,
 *	só envia quando o registrador TSR está
 *	vazio (bit TMRT = 1)
 *
 *	@param to_send caractere a ser enviado
 *
 *	@register TXREG = to_send	: envia to_send através da porta serial
 **/

void USART_send_char(unsigned int8 to_send)
{
        while (!(TXSTA & (1 << 1)));   // while (TMRT != 1)
        TXREG = to_send;               // envia dados <!> verificar tipo
}

/**
 *	USART_send_string
 *
 *	envia uma string pela porta serial
 *	suando USART_send_char
 *
 *	@param s string a ser enviada
 *
 *	@register RCSTA inalterado
 *	@register TXSTA inalterado
 **/

void USART_send_string(char *s)
{
	unsigned int16 n, i;
      	
      	n = strlen(s);

	RCSTA |= (1 << 7);        // enable SERIAL PORT 
	TXSTA |= (1 << 5);        // enable TRANSMISSION
	
	for (i = 0; i < n; ++i)
		USART_send_char(s[i]);
	
	RCSTA &= ~(1 << 7);        // disable SERIAL PORT 
	TXSTA &= ~(1 << 5);        // disable TRANSMISSION
}

/**
 *	USART_reception_enable
 *
 *	configura a porta serial para recepção de dados
 **/

void USART_recept_enable()
{
	RCSTA |= (1 << 5);
	RCSTA |= (1 << 4);
}

char USART_get_char()
{
        char to_send;

        while (!(PIR1 & (1 << 5)));
        to_send = RCREG;
        
        return to_send;
}

/**
 *	USART_recept_disable
 *
 *	'desliga' recepção de dados
 **/

void USART_recept_disable()
{
	RCSTA &= ~(1 << 5);
	RCSTA &= ~(1 << 4);
}
