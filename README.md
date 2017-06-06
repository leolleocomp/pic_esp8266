# Código utilizado em um projeto da disciplina de sistemas microcontrolados

Programa escrito para PIC16F876A, lê dados 3 sensores de 
temperatura, luminosidade e umidade e envia-os periodicamente
(período a definir) periodicamente ao esp8266, que envia-os
para o ThingSpeak.

Todo:

- [x] comunicação pela USART
- [x] setup timer1
- [x] setup interrupção do timer1
- [x] setup ADC read
- [x] setup comunicação com o esp8266


Team:

* Leonardo Cavalcante do Prado (leolleocomp)
* Esron Dtamar
* Gabriel Rafael

Computer engineering undergraduates at UNIVASF - Federal
University of Vale do São Francisco.
