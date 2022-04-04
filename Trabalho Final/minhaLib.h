#ifndef MINHALIB_H
#define MINHALIB_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/Nokia5110.h"


#define B1 0b0011100
#define B2 0b0011010
#define B3 0b0010110
#define B4 0b0001110
#define B5 0b0000001
#define B6 0b0010000


void IntPortalE(void);
void intPortalF(void);
void configuraGPIO();
void imprimeTabela();
void imprimeSeta(int posicao);
int imprimeSimbolo(int x, int y, char simb);
void trocaVez();
char testaVencedor();
void reiniciaJogo();
void carregaTelaInical();


#endif
