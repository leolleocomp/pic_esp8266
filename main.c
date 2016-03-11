#include <16F876A.h>
#fuses HS,NOLVP,NOWDT,NOPUT,PROTECT,NOBROWNOUT,DEBUG,NOCPD
#device ADC=10
#use delay(clock = 20000000)
#include <string.h>
#include <registers.h>
#include <timer1.h>
#include <USART.h>

int main()
{
        char msg[] = { "essa mensagem nao possui o menor sentido... opa opa opa opa opa opa \r\n\0" };
        USART_config();
        
        while(true) {
                delay_ms(1000);
                USART_send_string(msg);
        }
}
