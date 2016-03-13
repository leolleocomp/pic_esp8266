/**
 *        esp8266.h
 *
 *        conjunto de funções relacionadas ao esp8266
 **/


char esp8266_get_char()
{
        char to_send;

        while (!(PIR1 & (1 << 5)));
        to_send = RCREG;
        
        return to_send;
}

void esp8266_wait_response(char *ans)
{
        int16 n, i;
        n = strlen(ans);        

        for (i = 0; i < n; ++i)
                if (esp8266_get_char() != ans[i])
                        return;
}

/**
 *        esp8266_config
 *
 *        configura o esp8266 no modo estação,
 *        conecta a um roteador
 *        configura multiplas conexões,
 *
 */

void esp8266_config()
{
        char s[20], ok[6] = { "OK" };
        
        USART_recept_enable();

        strcpy(s, "AT+CWMODE=1\r\n\0");
        USART_send_string(s);
        esp8266_wait_response(ok);

        strcpy(s, "AT+CWSAP=\"xxxx\",\"xxxx\"\r\n\0");
        USART_send_string(s);
        esp8266_wait_response(ok);

        strcpy(s, "AT+CIPMUX=1\r\n\0");
        USART_send_string(s);
        esp8266_wait_response(ok);

        USART_recept_disable();
}

void esp8266_open_tcp()
{
        char s[50], ok[4] = { "OK" };

        USART_recept_enable();

        strcpy(s, "AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80\r\n\0");
        USART_send_string(s);
        esp8266_wait_response(ok);

        USART_recept_disable();
}

void esp8266_send_data()
{
        int16 n;
        char s[80], ok[10] = { "SEND OK" };
        n = strlen(s);

        USART_recept_enable();        

        strcpy(s, "AT+CIPSEND=4,71\r\n\0");
        USART_send_string(s);
        esp8266_wait_response(ok);

        strcpy(s, "GET /update?key=SE27NFVOT83ISQ3Y&field1=%04ld");
        sprintf(s, s, temperatura);
        USART_send_string(s);

        strcpy(s, "&field2=%04ld&field3=%04ld\r\n\0");
        sprintf(s, s, luminosidade, umidade);
        USART_send_string(s);
        delay_ms(5000);

        USART_recept_disable();
}
