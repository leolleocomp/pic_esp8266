#include <16F876A.h>
#fuses HS,NOLVP,NOWDT,NOPUT,PROTECT,NOBROWNOUT,DEBUG,NOCPD
#device ADC=10
#use delay(clock = 20000000)

#define TMR1L_START_VALUE 0b11011100
#define TMR1H_START_VALUE 0b00001011

long  temperatura  = 0,
      luminosidade = 0,
      umidade      = 0;
      
#include <string.h>
#include <registers.h>

unsigned int8 espPrepareSend[]     = { "AT+CIPSEND=4,71\r\n\0" },
              espGETcmd[]          = { "GET /update?key=SE27NFVOT83ISQ3Y" };

int16 send_data_en = 0, cont = 0;

void USART_send_char(unsigned int8 to_send)
{
        while (!(TXSTA & (1 << 1)));   // while (TMRT != 1)
        TXREG = to_send;               // envia dados <!> verificar tipo
}

void USART_send_string(char *s)
{
        unsigned int16 n, i;
              
        n = strlen(s);
              
        for (i = 0; i <= n; ++i)
                USART_send_char(s[i]);
}

void esp8266_config()
{                
        unsigned int8        espMode[]            = { "AT+CWMODE=1\r\n\0" },
                              espConnectAp[]       = { "AT+CWJAP=\"xxxx\",\"xxxx\"\r\n\0" },
                              espMultConnections[] = { "AT+CIPMUX=1\r\n\0" };
        
        USART_send_string(espMode);
        delay_ms(500);
     
        USART_send_string(espConnectAp);
        delay_ms(500);

   
        USART_send_string(espMultConnections);
        delay_ms(500);
}

void esp8266_open_tcp()
{
         unsigned int8 espOpenTCP[] = { "AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80\r\n\0" };
         
        USART_send_string(espOpenTCP);
        delay_ms(5000);
}

void esp8266_send_data()
{
        unsigned int8 tmp[90];

        USART_send_string(espPrepareSend);
        delay_ms(500);

        sprintf(tmp, "%s&field1=%04ld&field2=%04lds&field3=%04ld\r\n\0", espGETcmd, temperatura, luminosidade, umidade);
        USART_send_string(tmp);
        delay_ms(5000);
       // USART_send_string(tmp);
       // delay_ms(5000);
}

void main()
{
        unsigned int8 op[4] = {"\r\n\0"};
        
        TXSTA  = 0b00100100;  
        SPBRG  = 129;        
        RCSTA  = 0b10000000;
        TRISC  = 0b11111111;
        
        USART_send_string(op);
         
	delay_ms(10);
	esp8266_config();
        
        T1CON  = 0b00000000;        
        INTCON = 0b11000000; 
        PIE1   = 0b00000001;
        
        TMR1L = TMR1L_START_VALUE;
        TMR1H = TMR1H_START_VALUE;
        T1CON |= 1;

        while(true) {
        	delay_ms(1000);
        	send_data_en = 1;
        	
                if (send_data_en == 1) {
                        T1CON &= ~1;
                        
                        esp8266_open_tcp();
                        esp8266_send_data();
                        send_data_en = 0;
                        
                        //TMR1L = TMR1L_START_VALUE;
                        //TMR1H = TMR1H_START_VALUE;
                        T1CON |= 1;
                }
                
                //TXREG = 'x';
        }
}

#int_timer1
void isr_timer1()
{
        cont++;
       // TXREG = 'z';
        if (cont >= 76) {
                send_data_en = 1;
                cont = 0;
        }
       //USART_send_char('x');
      // TXREG='x';
       // while (!(TXSTA & 2));
       // TXREG = '\0';
        
       // TMR1L = TMR1L_START_VALUE;
       // TMR1H = TMR1H_START_VALUE;
}
