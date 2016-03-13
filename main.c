#include <16F876A.h>
#fuses HS,NOLVP,NOWDT,NOPUT,PROTECT,NOBROWNOUT,DEBUG,NOCPD
#device ADC=10
#use delay(clock = 20000000)

int16 temperatura  = 0,
      luminosidade = 0,
      umidade      = 0;
      
#include <string.h>
#include <registers.h>
#include <timer1.h>
#include <USART.h>
#include <esp8266.h>

int16 send_data_en = 0, cont = 0;

void main()
{
        USART_config();
	esp8266_config();
        timer1_config();

        timer1_set_start_value();
        timer1_enable();
        
        while(true) {
                if (send_data_en) {
			esp8266_open_tcp();
			esp8266_send_data();
                        send_data_en = 0;
                }

        }
}

#int_timer1
void isr_timer1()
{
        cont++;

        if (cont >= 300) {
                send_data_en = 1;
                cont = 0;
        }
        
//	timer1_set_start_value();
        PIR1 &= ~1;
}
