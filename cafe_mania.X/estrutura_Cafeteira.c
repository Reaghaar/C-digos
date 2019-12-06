#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pic18f4520.h>
#include "configura.h"
#include "lcd.h"

#define _XTAL_FREQ 4000000                          //Define a frequencia do pic em 4 Mhz
#define agua PORTCbits.RC0                          //Define o RC0 do PORTC com nome agua
#define b0  PORTCbits.RC1                           //bot�o 1 - caf� curto
#define b1  PORTCbits.RC2                           //bot�o 2 - caf� expresso
#define b2  PORTCbits.RC3                           //bot�o 3 - capuccino
int EXEC;
int tem_agua;                                       //Vari�vel auxiliar de verifica��o de nivel de �gua
void nivel_agua(void);                              //Fun��o para verifica��o de nivel de �gua
void trata (char RECEP);                            //Fun��o onde ocorre os tratamentos das fun��es dos tipos de bebidas
void cafe(void);                                    //Fun��o onde trata a sele��o do caf�
void cafe_expresso(void);                           //Fun��o onde trata a sele��o do caf� expresso
void capuccino(void);                               //Fun��o onde trata a sele��o do capuccino
void botao(void);                                   //Fun��o dos bot�es f�sicos
void envia(int n);                                  //Fun��o para enviar dados do pic em comunica��o serial

void main(void)                                     //Fun��o principal
{
    config();                                      	//Chamada da fun��o config da biblioteca configura.h
    lcd_init();                                     //Chama da fun��o lcd_init que inicializa o lc da biblioteca lcd.h
    
    PORTB   = 0;
    PORTD   = 0;
    PORTE   = 0;
    LATA5   = 1;                                    //Acende a luz do LCD
    
    while(1)                                       	//Loopin principal
    {       
        nivel_agua();								//Verifica constantemente o nivel da �gua
        lcd_clear();                                //Limpa lcd
        lcd_goto(3);                                //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
        lcd_puts("Escolha");                        //Insere palavra "Escolha"
        lcd_goto(40);                               //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
        lcd_puts("   sua bebida");                  //insere frase "   sua bebida"
        __delay_ms(50);                             //Delay para atualiza�ao do display
             
        botao();
    }
}
void envia(int n)                                   //Envia dado TX
{
    while(!TXIF){}
    if (n == 10)
    {
        TXREG = 10;
    }
    else if (n==20)
    	    TXREG = 20;
    else if (n==5)
    	    TXREG = 5;
    else if(n==15)
    	    TXREG = 15;
}
void __interrupt(high_priority) tmr (void)          //fun��o de prioridade
{
    if(RCIF)                                        //vefifica a flag de interrup��o RCIF para conex��o RX
    {
       RCIF = 0;                                    //Limpa a interrup��o 
       EXEC = RCREG;								//A vari�vel EXEC recebe os dados de RCREG
       trata(EXEC);									//Recebe um caracter para comunica��o serial
	}

}
void nivel_agua(void)                               //fun��o de verifica��o de nivel de �gua
{
    if(!agua)                                       //  |
    {                                               //  |
        __delay_ms(50);                             //  |- Filtro de bounce para o bot�o agua
        if(!agua)                                   //  |
        {                                           //  |
            while(!agua)                            //
            {
                lcd_clear();                        //Limpa lcd
                lcd_goto(3);                        //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
                lcd_puts("Baixo nivel");            //Insere frase "Baixo nivel"
                lcd_goto(40);                       //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
                lcd_puts("     de agua");           //insere frase "     de agua"
                __delay_ms(50);                     //Delay para atualiza�ao do display
                envia(15);
            }
            tem_agua = 0;                          	//aux�iliar de verifica��o que indica que n�o tem �gua
        }
    }
    else
    {
        tem_agua = 1;                               //auxiliar de verifica��o que indica que tem �gua 
        lcd_clear();								//Limpa cache no display
    }
}
void trata(char RECEP)                              //Fun��o que decide qual condi��o deve ser selecionada
{            
        switch(RECEP)								//Fun��o para tratamento dos dados da cominica��o serial		
        {
        case 'a':									//Condi��o de aceita��o do tipo do caf�
            cafe();									//Entra na fun��o desejada
            break;
        case 'b':									//Condi��o de aceita��o do tipo do caf�
            cafe_expresso();						//Entra na fun��o desejada
            break;
        case 'c':									//Condi��o de aceita��o do tipo do caf�
            capuccino();							//Entra na fun��o desejada
            break;
        }
}
void cafe(void)                                     //Fun��o que prepara o caf�
{
    if(tem_agua==1)                                 //caso tenha agua libera as fun��es de preparo do caf�
    {      
		for(int i=0;i<5;i++)
        {
            lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
            lcd_puts("Aquecendo");                  //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
            lcd_puts("     A Agua");                    //insere frase "    A Agua"
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 1;                      //  |- Pisca o led vermelho indicando que o caf� esta sendo preparado
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 0;                      //  |
            envia(5);
	   }
       for(int i = 0;i <5;i++)                      //Loopin que indica que o caf� est� pronto
       {
			lcd_clear();                            //Limpa lcd
			lcd_goto(3);                            //co�loca o ponteiro na posi��o dos superior esquedo dos leds do lcd
			lcd_puts("Preparando");                 //Insere frase "Preparando"
			lcd_goto(40);                           //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
			lcd_puts("Cafe Curto");                 //insere frase "Cafe Curto"
			__delay_ms(500);                        //  |
			PORTBbits.RB6 = 1;                      //  |- Pisca �o led vermelho indicando que o caf� esta sendo preparado
			__delay_ms(500);                        //  |
       	    PORTBbits.RB6 = 0;                      //  |
            envia(10);
        }     
        for(int i = 0;i <4;i++)                     //Loopin que indica que o caf� est� pronto
        {
       	    lcd_clear();                            //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
       	    lcd_goto(3);                            
			lcd_puts("Cafe pronto!");               //Insere frase "Cafe pronto!"
	        __delay_ms(500);                        //  |
    	    PORTBbits.RB0 = 1;                      //  |- Pisca o led verde indicando que o caf� esta sendo preparado
			__delay_ms(500);                        //  |
            PORTBbits.RB0 = 0;                      //  |
            envia(20);
         }
    }
    else                                            //Caso n�o tenha agua verifica se tem �gua
    {
        nivel_agua();               
    }
}
void cafe_expresso(void)                           	//Fun��o que prepara o caf� expresso
{
    if(tem_agua==1)                                	//caso tenha agua libera as fun��es de preparo do caf� expresso
    {
		for(int i=0;i<5;i++)
        {
			lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
            lcd_puts("Aquecendo");                  //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
            lcd_puts("     A Agua");                    //insere frase "    A Agua"
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 1;                      //  |- Pisca o led vermelho indicando que o caf� esta sendo preparado
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 0;                      //  |
            envia(5);
        }
        for(int i = 0;i <5;i++)                     //Loopin que indica que o caf� est� pronto
        {
            lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
            lcd_puts("Preparando");                 //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
            lcd_puts(" Cafe Expresso");             //insere frase " Cafe Expresso"
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 1;                     //  |- Pisca o led vermelho indicando que o caf� esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 0;                     //  |
             envia(10);
         } 
         for(int i = 0;i <4;i++)                    //Loopin que indica que o caf� est� pronto
         {
             lcd_clear();                           //Limpa lcd
             lcd_goto(3);                           //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
             lcd_puts("Cafe pronto!");              //Insere frase "Cafe pronto!"
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 1;                     //  |- Pisca o led verde indicando que o caf� esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 0;                     //  |
             envia(20);
         }
    }
    else										    //Caso n�o tenha agua verifica se tem �gua
    {
        nivel_agua();                               
    }
}
void capuccino(void)                                	
{
    if(tem_agua==1)                                	//caso tenha agua libera as fun��es de preparo do caf�
    {
 		for(int i=0;i<5;i++)
        {
			lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
            lcd_puts("Aquecendo");                  //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
            lcd_puts("     A Agua");                //insere frase "    A Agua"
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 1;                      //  |- Pisca o led vermelho indicando que o caf� esta sendo preparado
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 0;                      //  |
            envia(5);
        }
        for(int i = 0;i <5;i++)                     //Loopin que indica que o caf� est� pronto
        {
            lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
            lcd_puts("Preparando");                 //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posi��o dos inferior esquerdo dos leds do lcd
            lcd_puts("  Capuccino");                //insere frase " Cafe Expresso"
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 1;                     //  |- Pisca o led vermelho indicando que o caf� esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 0;                     //  |
             envia(10);
         } 
         for(int i = 0;i <4;i++)                    //Loopin que indica que o caf� est� pronto
         {
             lcd_clear();                           //Limpa lcd
             lcd_goto(3);                           //coloca o ponteiro na posi��o dos superior esquedo dos leds do lcd
             lcd_puts("Capuccino pronto!");         //Insere frase "Cafe pronto!"
             lcd_goto(40);
             lcd_puts("pronto!");
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 1;                     //  |- Pisca o led verde indicando que o caf� esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 0;                     //  |
             envia(20);
         }
    }
    else										    //Caso n�o tenha agua verifica se tem �gua
    {
        nivel_agua();
    }
}
void botao(void)									//Fun��o que permite a escolha dos caf�s por comunica��o serial
{
    if(tem_agua==1)									//Caso tenha agua libera as fun��es de preparo do caf�
	{
		if(!b0)										//Faz a verifica��o do bot�o RC0 para aceita��o da fun��o caf�
		{
			__delay_ms(5);
			if(!b0)
			{
			    while(!b0);
			    cafe();								//Entra na fun��o do caf�
			}
		}
		if(!b1)										//Faz a verifica��o do bot�o RC0 para aceita��o da fun��o caf�
		{
			__delay_ms(5);
			if(!b1)
			{
			    while(!b1);
			    cafe_expresso();					//Entra na fun��o do caf�_expresso
			}
		}
		if(!b2)										//Faz a verifica��o do bot�o RC0 para aceita��o da fun��o caf�
		{
			__delay_ms(5);
			if(!b2)
			{
			    while(!b2);
			    capuccino();						//Entra na fun��o do capuccino
			}
		}
	}
	else											//Caso n�o tenha agua verifica se tem �gua
	{
		nivel_agua();
	}
}