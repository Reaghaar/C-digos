#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pic18f4520.h>
#include "configura.h"
#include "lcd.h"

#define _XTAL_FREQ 4000000                          //Define a frequencia do pic em 4 Mhz
#define agua PORTCbits.RC0                          //Define o RC0 do PORTC com nome agua
#define b0  PORTCbits.RC1                           //botão 1 - café curto
#define b1  PORTCbits.RC2                           //botão 2 - café expresso
#define b2  PORTCbits.RC3                           //botão 3 - capuccino
int EXEC;
int tem_agua;                                       //Variável auxiliar de verificação de nivel de água
void nivel_agua(void);                              //Função para verificação de nivel de água
void trata (char RECEP);                            //Função onde ocorre os tratamentos das funções dos tipos de bebidas
void cafe(void);                                    //Função onde trata a seleção do café
void cafe_expresso(void);                           //Função onde trata a seleção do café expresso
void capuccino(void);                               //Função onde trata a seleção do capuccino
void botao(void);                                   //Função dos botões físicos
void envia(int n);                                  //Função para enviar dados do pic em comunicação serial

void main(void)                                     //Função principal
{
    config();                                      	//Chamada da função config da biblioteca configura.h
    lcd_init();                                     //Chama da função lcd_init que inicializa o lc da biblioteca lcd.h
    
    PORTB   = 0;
    PORTD   = 0;
    PORTE   = 0;
    LATA5   = 1;                                    //Acende a luz do LCD
    
    while(1)                                       	//Loopin principal
    {       
        nivel_agua();								//Verifica constantemente o nivel da água
        lcd_clear();                                //Limpa lcd
        lcd_goto(3);                                //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
        lcd_puts("Escolha");                        //Insere palavra "Escolha"
        lcd_goto(40);                               //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
        lcd_puts("   sua bebida");                  //insere frase "   sua bebida"
        __delay_ms(50);                             //Delay para atualizaçao do display
             
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
void __interrupt(high_priority) tmr (void)          //função de prioridade
{
    if(RCIF)                                        //vefifica a flag de interrupção RCIF para conexção RX
    {
       RCIF = 0;                                    //Limpa a interrupção 
       EXEC = RCREG;								//A variável EXEC recebe os dados de RCREG
       trata(EXEC);									//Recebe um caracter para comunicação serial
	}

}
void nivel_agua(void)                               //função de verificação de nivel de água
{
    if(!agua)                                       //  |
    {                                               //  |
        __delay_ms(50);                             //  |- Filtro de bounce para o botão agua
        if(!agua)                                   //  |
        {                                           //  |
            while(!agua)                            //
            {
                lcd_clear();                        //Limpa lcd
                lcd_goto(3);                        //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
                lcd_puts("Baixo nivel");            //Insere frase "Baixo nivel"
                lcd_goto(40);                       //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
                lcd_puts("     de agua");           //insere frase "     de agua"
                __delay_ms(50);                     //Delay para atualizaçao do display
                envia(15);
            }
            tem_agua = 0;                          	//aux iliar de verificação que indica que não tem água
        }
    }
    else
    {
        tem_agua = 1;                               //auxiliar de verificação que indica que tem água 
        lcd_clear();								//Limpa cache no display
    }
}
void trata(char RECEP)                              //Função que decide qual condição deve ser selecionada
{            
        switch(RECEP)								//Função para tratamento dos dados da cominicação serial		
        {
        case 'a':									//Condição de aceitação do tipo do café
            cafe();									//Entra na função desejada
            break;
        case 'b':									//Condição de aceitação do tipo do café
            cafe_expresso();						//Entra na função desejada
            break;
        case 'c':									//Condição de aceitação do tipo do café
            capuccino();							//Entra na função desejada
            break;
        }
}
void cafe(void)                                     //Função que prepara o café
{
    if(tem_agua==1)                                 //caso tenha agua libera as funções de preparo do café
    {      
		for(int i=0;i<5;i++)
        {
            lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
            lcd_puts("Aquecendo");                  //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
            lcd_puts("     A Agua");                    //insere frase "    A Agua"
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 1;                      //  |- Pisca o led vermelho indicando que o café esta sendo preparado
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 0;                      //  |
            envia(5);
	   }
       for(int i = 0;i <5;i++)                      //Loopin que indica que o café está pronto
       {
			lcd_clear();                            //Limpa lcd
			lcd_goto(3);                            //co loca o ponteiro na posição dos superior esquedo dos leds do lcd
			lcd_puts("Preparando");                 //Insere frase "Preparando"
			lcd_goto(40);                           //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
			lcd_puts("Cafe Curto");                 //insere frase "Cafe Curto"
			__delay_ms(500);                        //  |
			PORTBbits.RB6 = 1;                      //  |- Pisca  o led vermelho indicando que o café esta sendo preparado
			__delay_ms(500);                        //  |
       	    PORTBbits.RB6 = 0;                      //  |
            envia(10);
        }     
        for(int i = 0;i <4;i++)                     //Loopin que indica que o café está pronto
        {
       	    lcd_clear();                            //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
       	    lcd_goto(3);                            
			lcd_puts("Cafe pronto!");               //Insere frase "Cafe pronto!"
	        __delay_ms(500);                        //  |
    	    PORTBbits.RB0 = 1;                      //  |- Pisca o led verde indicando que o café esta sendo preparado
			__delay_ms(500);                        //  |
            PORTBbits.RB0 = 0;                      //  |
            envia(20);
         }
    }
    else                                            //Caso não tenha agua verifica se tem água
    {
        nivel_agua();               
    }
}
void cafe_expresso(void)                           	//Função que prepara o café expresso
{
    if(tem_agua==1)                                	//caso tenha agua libera as funções de preparo do café expresso
    {
		for(int i=0;i<5;i++)
        {
			lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
            lcd_puts("Aquecendo");                  //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
            lcd_puts("     A Agua");                    //insere frase "    A Agua"
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 1;                      //  |- Pisca o led vermelho indicando que o café esta sendo preparado
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 0;                      //  |
            envia(5);
        }
        for(int i = 0;i <5;i++)                     //Loopin que indica que o café está pronto
        {
            lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
            lcd_puts("Preparando");                 //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
            lcd_puts(" Cafe Expresso");             //insere frase " Cafe Expresso"
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 1;                     //  |- Pisca o led vermelho indicando que o café esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 0;                     //  |
             envia(10);
         } 
         for(int i = 0;i <4;i++)                    //Loopin que indica que o café está pronto
         {
             lcd_clear();                           //Limpa lcd
             lcd_goto(3);                           //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
             lcd_puts("Cafe pronto!");              //Insere frase "Cafe pronto!"
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 1;                     //  |- Pisca o led verde indicando que o café esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 0;                     //  |
             envia(20);
         }
    }
    else										    //Caso não tenha agua verifica se tem água
    {
        nivel_agua();                               
    }
}
void capuccino(void)                                	
{
    if(tem_agua==1)                                	//caso tenha agua libera as funções de preparo do café
    {
 		for(int i=0;i<5;i++)
        {
			lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
            lcd_puts("Aquecendo");                  //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
            lcd_puts("     A Agua");                //insere frase "    A Agua"
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 1;                      //  |- Pisca o led vermelho indicando que o café esta sendo preparado
            __delay_ms(500);                        //  |
            PORTBbits.RB7 = 0;                      //  |
            envia(5);
        }
        for(int i = 0;i <5;i++)                     //Loopin que indica que o café está pronto
        {
            lcd_clear();                            //Limpa lcd
            lcd_goto(3);                            //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
            lcd_puts("Preparando");                 //Insere frase "Preparando"
            lcd_goto(40);                           //coloca o ponteiro na posição dos inferior esquerdo dos leds do lcd
            lcd_puts("  Capuccino");                //insere frase " Cafe Expresso"
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 1;                     //  |- Pisca o led vermelho indicando que o café esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB6 = 0;                     //  |
             envia(10);
         } 
         for(int i = 0;i <4;i++)                    //Loopin que indica que o café está pronto
         {
             lcd_clear();                           //Limpa lcd
             lcd_goto(3);                           //coloca o ponteiro na posição dos superior esquedo dos leds do lcd
             lcd_puts("Capuccino pronto!");         //Insere frase "Cafe pronto!"
             lcd_goto(40);
             lcd_puts("pronto!");
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 1;                     //  |- Pisca o led verde indicando que o café esta sendo preparado
             __delay_ms(500);                       //  |
             PORTBbits.RB0 = 0;                     //  |
             envia(20);
         }
    }
    else										    //Caso não tenha agua verifica se tem água
    {
        nivel_agua();
    }
}
void botao(void)									//Função que permite a escolha dos cafés por comunicação serial
{
    if(tem_agua==1)									//Caso tenha agua libera as funções de preparo do café
	{
		if(!b0)										//Faz a verificação do botão RC0 para aceitação da função café
		{
			__delay_ms(5);
			if(!b0)
			{
			    while(!b0);
			    cafe();								//Entra na função do café
			}
		}
		if(!b1)										//Faz a verificação do botão RC0 para aceitação da função café
		{
			__delay_ms(5);
			if(!b1)
			{
			    while(!b1);
			    cafe_expresso();					//Entra na função do café_expresso
			}
		}
		if(!b2)										//Faz a verificação do botão RC0 para aceitação da função café
		{
			__delay_ms(5);
			if(!b2)
			{
			    while(!b2);
			    capuccino();						//Entra na função do capuccino
			}
		}
	}
	else											//Caso não tenha agua verifica se tem água
	{
		nivel_agua();
	}
}