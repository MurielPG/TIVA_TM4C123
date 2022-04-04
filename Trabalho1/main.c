#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"

#define HIGH 0xFF
#define LOW 0x00
#define Red GPIO_PIN_1
#define Blue GPIO_PIN_2
#define Green GPIO_PIN_3
#define b1 GPIO_PIN_4
#define b2 GPIO_PIN_0




uint32_t Periodo, Duracao, tempo_triac, taxa = 0, lock = 1;
float delay;

void Timer0IntHandler(void)         //pulso rede
{

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTF_BASE, Green, HIGH);   //Desativa o led verde

    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE, TIMER_A);    //Liga o led verde na duracao

    if(lock == 0){
        lock = 1;

        TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
        TimerEnable(TIMER4_BASE, TIMER_A);    //Inicia o timer 4(Delay)

        TimerIntDisable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
        TimerDisable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);    //Desativa o TRIAC
    }
    GPIOIntEnable(GPIO_PORTF_BASE, b1);  //Liga o botão 1
}



void Timer1IntHandler(void)         //Duração do pulso verde
{
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTF_BASE, Green, LOW);   //Ativa o led verde

    TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}



void Timer2IntHandler(void)         //Pulso TRIAC
{
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTF_BASE, Blue, HIGH);   //Ativa o led azul

    TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER3_BASE, TIMER_A);    //Liga o led verde na duracao
}



void Timer3IntHandler(void)         //Duração do pulso TRIAC
{
    TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTF_BASE, Blue, LOW);   //Desativa o led azul

    TimerIntDisable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
}



void Timer4IntHandler(void)         //tempo triac
{
    TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER2_BASE, TIMER_A);

    TimerDisable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
}



void IntPortalF (void){

    GPIOIntClear(GPIO_PORTF_BASE, b1|b2);

    uint8_t source = GPIOPinRead(GPIO_PORTF_BASE, b1|Green|Blue|Red|b2);

    if ((source & b2)==LOW)
    {

        TimerEnable(TIMER0_BASE, TIMER_A);
        TimerEnable(TIMER2_BASE, TIMER_A);

    }

    if ((source & b1) == LOW)
    {

        taxa = taxa + 1;

        if(taxa == 5){
            taxa = 0;
        }

        tempo_triac = Periodo*(0.2)*taxa;

        TimerLoadSet(TIMER4_BASE, TIMER_A, (SysCtlClockGet()/1)/1*0.2);

        GPIOIntClear(GPIO_PORTF_BASE, b1);

        GPIOIntDisable(GPIO_PORTF_BASE, b1);

        lock = 0;

    }

    GPIOIntDisable(GPIO_PORTF_BASE, b2);

}



int main(void)
{

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, b1|b2);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, Red|Blue|Green);



    Periodo = (SysCtlClockGet() / 1)/1;
    Duracao = (SysCtlClockGet() / 1)/20;



    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, Periodo-1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);

    TimerConfigure(TIMER1_BASE, TIMER_CFG_A_ONE_SHOT);
    TimerLoadSet(TIMER1_BASE, TIMER_A, Duracao-1);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);



    TimerConfigure(TIMER2_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER2_BASE, TIMER_A, Periodo-1);
    IntEnable(INT_TIMER2A);
    TimerIntEnable(TIMER2_BASE,TIMER_TIMA_TIMEOUT);

    TimerConfigure(TIMER3_BASE, TIMER_CFG_A_ONE_SHOT);
    TimerLoadSet(TIMER3_BASE, TIMER_A, Duracao-1);
    IntEnable(INT_TIMER3A);
    TimerIntEnable(TIMER3_BASE,TIMER_TIMA_TIMEOUT);



    TimerConfigure(TIMER4_BASE, TIMER_CFG_A_ONE_SHOT);
    TimerLoadSet(TIMER4_BASE, TIMER_A, 0);
    IntEnable(INT_TIMER4A);
    TimerIntEnable(TIMER4_BASE,TIMER_TIMA_TIMEOUT);



    GPIOPadConfigSet(GPIO_PORTF_BASE, b1|b2, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, b2|b1, GPIO_FALLING_EDGE);
    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE, b2);
    IntMasterEnable();

    while(1)
    {
    }

}
