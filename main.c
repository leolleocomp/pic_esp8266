#include <16F876A.h>
#fuses HS,NOLVP,NOWDT,NOPUT,PROTECT,NOBROWNOUT,DEBUG,NOCPD
#device ADC=10
#use delay(clock = 20000000)

#define TMR1L_START_VALUE 0b11011100
#define TMR1H_START_VALUE 0b00001011

#define LUMINOSIDADE 0
#define TEMPERATURA  1
#define UMIDADE      2

int16   sensor[3];
float32 values[3];
      
#include <string.h>
#include <registers.h>

unsigned int8 espPrepareSend[] = { "AT+CIPSEND=4,71\r\n\0" },
              espGETcmd[]      = { "GET /update?key=SE27NFVOT83ISQ3Y" };

long send_data_en = 0,  // enable send data
     cont = 0;         // controls the timming

/**
 *	start_signal
 *
 *	starts communication with the sensor
 **/

void start_signal()
{
	TRISA &= ~(1 << 1);    //Configure RA1 as output
	PORTA &= ~(1 << 1);    //RA1 sends 0 to the sensor
	delay_ms(18);
	PORTA |= (1 << 1);     //RA1 sends 1 to de sensor
	delay_us(30);
	TRISA |= (1 << 1);     // RA1 as input
}

/**
 *	check_response
 *
 *	checks the sensor
 **/

int8 check_response()
{
	int8 check;
	check = 0;
	delay_us(40);

	if (!(PORTA & (1 << 1))){
		delay_us(80);
		if (PORTA & (1 << 1))  check = 1;
		delay_us(40);
	}
}

/**
 *	read_data
 *
 *	reads 8 bits from sensor
 **/

int8 read_data()
{	
	int8 i, j;

	for(j = 0; j < 8; j++) {
		while (!(PORTA & (1 << 1))); //Wait until RA0 goes HIGH
		delay_us(30);

		if (!(PORTA & (1 << 1)))
		      i&= ~(1<<(7 - j));  	//Clear bit (7-b)
		else {
			i|= (1 << (7 - j));  	//Set bit (7-b)
			while(PORTA & (1 << 1));
		}  //Wait until RA0 goes LOW
	}
	return i;
}

/**
 *	read_RH_and_T
 *
 *	reads umidity and temperature
 *	data through dht22
 **/

void read_RH_and_T()
{
	int8 check,
	     RH_byte1,
	     RH_byte2,
	     T_byte1,
	     T_byte2,
	     check_sum;

	start_signal();
	check = check_response();

	if (check == 1) {
		RH_byte1  = read_data();
		RH_byte2  = read_data();
		T_byte1   = read_data();
		T_byte2   = read_data();	
		check_sum = read_data();

		if (check_sum == (RH_byte1 + RH_byte2 + T_byte1 + T_byte2) & 0xFF) {
			sensor[UMIDADE]   = RH_byte1;	
			sensor[UMIDADE] <<= 8;
		        sensor[UMIDADE]  |= RH_byte2;	

			sensor[TEMPERATURA]   = T_byte1;
			sensor[TEMPERATURA] <<= 8;
			sensor[TEMPERATURA]  |= T_byte2;
		}
	}
}

/**
 *        read_ADC
 *
 *        read sensor data:
 *	
 *	  reads luminosity sensor data
 *
 *        <!> to do: test this function
 **/

void readADC()
{      
	delay_us(50);                   // waits Tad ~ 64Tosc (for clk = 20MHz)
	ADCON0 |= (1 << 2);             // GO = 1
	sensor[LUMINOSIDADE] = 0;

	while (ADCON0 & (1 << 2));   // while !(~DONE) continue

	sensor[LUMINOSIDADE]   = ADRESH;   // 8 most significant bits
	sensor[LUMINOSIDADE] <<= 8;
	sensor[LUMINOSIDADE]  |= ADRESL;   // 8 least significant bits
}

/**
 *        USART_send_char
 *
 *        when TMRT == 1, sends to_send through serial
 **/

void USART_send_char(unsigned int8 to_send)
{
        while (!(TXSTA & (1 << 1)));   // while (TMRT != 1)
        TXREG = to_send;               // envia dados <!> verificar tipo
}

/**
 *        USART_send_string
 *
 *        sends a given string s through serial
 **/

void USART_send_string(char *s)
{
        unsigned int16 n, i;
              
        n = strlen(s);
              
        for (i = 0; i <= n; ++i)
                USART_send_char(s[i]);
}

/**
 *        esp8266_config
 *
 *         configures esp8266 in STATION MODE,
 *         connect to a access point according to xxxx ssid and xxxx password,
 *         set multiple connections.
 *
 *         <!> todo : configure time delays
 **/

void esp8266_config()
{                
        unsigned int8 espMode[]            = { "AT+CWMODE=1\r\n\0" },
                      espConnectAp[]       = { "AT+CWJAP=\"xxxx\",\"xxxx\"\r\n\0" },
                      espMultConnections[] = { "AT+CIPMUX=1\r\n\0" };
        
        USART_send_string(espMode);
        delay_ms(2000);
     
        USART_send_string(espConnectAp);
        delay_ms(10000);

   
        USART_send_string(espMultConnections);
        delay_ms(2000);
}

/**
 *        esp8266_open_tcp
 *
 *         opens a tcp connection with the ThingSpeak API
 *
 *         <!> todo: configure the time delay
 **/

void esp8266_open_tcp()
{
         unsigned int8 espOpenTCP[] = { "AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80\r\n\0" };
         
        USART_send_string(espOpenTCP);
        delay_ms(5000);
}

/**
 *        esp8266_send_data
 *
 *        uses espGETcmd to send data to the TS
 *
 *        <!> todo: configure time delays        
 **/

void esp8266_send_data()
{
        unsigned int8 tmp[90];

        USART_send_string(espPrepareSend);
        delay_ms(5000);

        sprintf(tmp, "%s&field1=%04ld&field2=%04lds&field3=%04ld\r\n\0", espGETcmd, sensor[UMIDADE], sensor[LUMINOSIDADE], sensor[TEMPERATURA]);
        USART_send_string(tmp);
        delay_ms(5000);
}

void main()
{
        unsigned int8 op[4] = {"\r\n\0"};
       
        
        // USART configuration section
        TXSTA  = 0b00100100;  // TXEN = BRGH = 1
        SPBRG  = 129;         // 9600 baud rate => SPBRG = 129
        RCSTA  = 0b10000000;  // SPEN = 1
        TRISC  = 0b11111111;  // TRISC set, according with datasheed

        // ADC config 
        ADCON1 = 0b11000100;        // ADFM = ADCS2 = 1, Tad = 64 * Tosc
        ADCON0 = 0b10000001;
        TRISA  = 0b00001011;
	
        USART_send_string(op);
         
        delay_ms(100);

        // configure esp
        esp8266_config();
        
        // timer1 configuration section
        T1CON  = 0b00000000;        
        INTCON = 0b11000000;                // GEIE = PEIE1 = 1
        PIE1   = 0b00000001;                 // TMR1IE = 1
        //TMR1L  = TMR1L_START_VALUE;         // start_value = (3036)_10
        //TMR1H  = TMR1H_START_VALUE;
        T1CON |= 1;                         // enable timer1

        while(true) {
                delay_ms(1000);
                send_data_en = 1;
                
                if (send_data_en == 1) {
                        T1CON &= ~1;        // timer1 DISABLE
                        
                        esp8266_open_tcp();
                        readADC();
                        esp8266_send_data();
                        send_data_en = 0;
                        
                        //TMR1L = TMR1L_START_VALUE; 
                        //TMR1H = TMR1H_START_VALUE;
                        T1CON |= 1;        // timer1 ENABLE
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
        
       // TMR1L = TMR1L_START_VALUE;
       // TMR1H = TMR1H_START_VALUE;
}
