#include <16F876A.h>
#fuses HS,NOLVP,NOWDT,NOPUT,PROTECT,NOBROWNOUT,DEBUG,NOCPD
#device ADC=10
#use delay(clock = 20000000)

#define TMR1L_START_VALUE 0b11011100
#define TMR1H_START_VALUE 0b00001011

int16 temperatura  = 0,
      luminosidade = 0,
      umidade      = 0;
      
#include <string.h>
#include <registers.h>

unsigned int8 espMode[]            = { "AT+CWMODE=1\r\n\0" },
              espConnectAp[]       = { "AT+CWJAP=\"xxxx\",\"xxxx\"\r\n\0" },
              espMultConnections[] = { "AT+CIPMUX=1\r\n\0" },
              espOpenTCP[]         = { "AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80\r\n\0" },
              espPrepareSend[]     = { "AT+CIPSEND=4,71\r\n\0" },
              espGETcmd[]          = { "GET /update?key=SE27NFVOT83ISQ3Y&field1=%04ld&field2=%04ld&field3=%04ld\r\n\0" };

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
            USART_send_string(espMode);
        delay_ms(500);
     
        USART_send_string(espConnectAp);
        delay_ms(500);

   
        USART_send_string(espMultConnections);
        delay_ms(500);
}

void esp8266_open_tcp()
{
        USART_send_string(espOpenTCP);
        delay_ms(5000);
}

void esp8266_send_data()
{
        unsigned int8 tmp[80];

        USART_send_string(espPrepareSend);
        delay_ms(500);

        sprintf(tmp, espGETcmd, temperatura, luminosidade, umidade);
        USART_send_string(tmp);
        delay_ms(500);
}

void main()
{
        TXSTA  = 0b00000100;  
        SPBRG  = 129;        
        RCSTA  = 0b00000000;
        TRISC  = 0b11111111;
        RCSTA |= (1 << 7);        // enable SERIAL PORT 
        TXSTA |= (1 << 5);        // enable TRANSMISSION
        
        esp8266_config();
        
        T1CON  = 0b00000000;        
        INTCON = 0b11000000; 
        PIE1   = 0b00000001;
        
        TMR1L = TMR1L_START_VALUE;
        TMR1H = TMR1H_START_VALUE;
        T1CON |= 1;

        while(true) {
                if (send_data_en == 1) {
                         T1CON &= ~1;
                         
                        esp8266_open_tcp();
                        esp8266_send_data();
                        send_data_en = 0;
                        
                        TMR1L = TMR1L_START_VALUE;
                        TMR1H = TMR1H_START_VALUE;
                        T1CON |= 1;
                }
        }
}

#int_timer1
void isr_timer1()
{
        cont++;

        if (cont >= 76) {
                send_data_en = 1;
                cont = 0;
        }
        
        TMR1L = TMR1L_START_VALUE;
        TMR1H = TMR1H_START_VALUE;
}
