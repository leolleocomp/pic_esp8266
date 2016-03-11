/**
 *	timer1.h
 *
 *	conjunto de funções relacionadas ao timer1
 **/

// timer1_start = 3036

#define TMR1L_START_VALUE 0b11011100
#define TMR1H_START_VALUE 0b00001011

// timer1, querido por todos
// timer1, a lenda
// timer1, um caminho sem retorno
// timer1, o gordinho que não cabe em apenas um registrador
// timer1, que apesar do 1 é o segundo timer
// timer1, heroi da noite
// timer1, o pic pode contar contigo
// timer1, o timer0 setado
// timer1, antes do timer2
// timer1, antes timer do que nunca

/**
 *	timer1_config
 *
 *	seta configuração do timer1
 *
 * 	@register T1CON = 00110000
 * 	@bit T1CKPS1 = 1 	: timer1
 * 	@bit T1CKPS0 = 1 	: prescaler 1/8
 * 	@bit T1OSCEN = 0 	: oscilador do timer1 desligado
 * 	@bit T1SYNC  = X 	: don't care 
 * 	@bit TMR1CRS = 0	: seleciona oscilador interno
 * 	@bit TMR1ON  = 0	: timer1 desligado
 *
 * 	@register INTCON = 11000000 
 * 	@bit GIE    = 1		: enables global interrupt
 * 	@bit PEIE   = 1		: enables peripheral interrupt
 * 	@bit [0-5]  = 0		
 *
 * 	@register PIE1 = 00000001
 * 	@bit [1-7]  = 0
 * 	@bit TMR1IE = 1		: enables timer1 interrupt
 *
 **/

void timer1_config()
{
	T1CON  = 0b00110000;	
	INTCON = 0b11000000; 
	PIE1   = 0b00000001;
}

/**
 *	timer1_enable
 *
 *	turn on timer1
 *
 *	@register T1CON |= 1
 *	@bit 	  TMR1ON = 1	: turn on timer1 
 *
 **/

void timer1_enable()
{
	T1CON |= 1;
}

/**
 *	timer1_disable()
 *
 *	turn off timer1
 *
 *	@register T1CON &= ~1;
 *	@bit 	  TMR1ON =  0; 	: turn off timer1
 **/

void timer1_disable()
{
	T1CON &= ~1;
}

/**
 *	timer1_set_start_value
 *
 *	seta timer1 de acordo com os valores definidos
 *	pelas constantes TMR1L_START_VALUE e TMR1H_START_VALUE
 **/

void timer1_set_start_value()
{
	TMR1L = TMR1L_START_VALUE;
	TMR1H = TMR1H_START_VALUE;
}
