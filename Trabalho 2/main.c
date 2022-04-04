#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/Nokia5110.h"
#include "driverlib/systick.h"


int x = 0;
int y = 1;

// declaracao do tempo inicial
int segundo_0 = 55;
int segundo_1 = 53;

int minuto_0 = 51;
int minuto_1 = 50;

int hora_0 = 56;
int hora_1 = 49;

uint8_t ui8PinData=0;

int valor[9];

void converteHora()
{
     segundo_0 = valor[7];
     segundo_1 = valor[6];

     minuto_0 = valor[4];
     minuto_1 = valor[3];

     hora_0 = valor[1];
     hora_1 = valor[0];
}


void UARTIntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);
    int i=0;
    while(UARTCharsAvail(UART0_BASE) && (i<=9))
    {
        valor[i] = UARTCharGetNonBlocking(UART0_BASE);
        UARTCharPutNonBlocking(UART0_BASE, valor[i]);
        i++;
    }
    converteHora();
}



void imprimeDescricao()
{
    char texto[] = {'e','x','e','m','p','l','o', ' ',
                     'H','H',':','M','M',':','S','S',' '};
    int tamanho = sizeof(texto)/sizeof(texto[0]);
    int i=0;
    for (i=0; i<tamanho; i++)
    {
        UARTCharPut(UART0_BASE, texto[i]);
    }
}

void habilitaUART()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    UARTEnable(UART0_BASE);

    UARTFIFODisable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
    UARTFIFOEnable(UART0_BASE);

    UARTIntEnable(UART0_BASE, UART_INT_RX);


    UARTIntEnable(UART0_BASE, UART_INT_RX); //only enable RX and TX interrupts
    imprimeDescricao();
}

void imprimeRelogioGrafico()
{
    int segundo = (segundo_1 - 48)*10 + (segundo_0 - 48);
    int minuto = (minuto_1 - 48)*10 + (minuto_0 - 48);
    int hora = (hora_1 - 48)*10 + (hora_0 - 48);

    //imprime os segundos
    Nokia5110_Clear();
    Nokia5110_SetCursor(segundo,0);
    Nokia5110_OutChar(124);
    Nokia5110_SetCursor(0,1);
    //Nokia5110_OutChar(49);
    Nokia5110_OutString("^1");
    Nokia5110_SetCursor(42,1);
    Nokia5110_OutString("60s^");

    //imprime os minutos
    Nokia5110_SetCursor(minuto,2);
    Nokia5110_OutChar(124);
    Nokia5110_SetCursor(0,3);
    //Nokia5110_OutChar(49);
    Nokia5110_OutString("^1");
    Nokia5110_SetCursor(42,3);
    Nokia5110_OutString("60m^");

    //imprime as horas
    Nokia5110_SetCursor(hora,4);
    Nokia5110_OutChar(124);
    Nokia5110_SetCursor(0,5);
    //Nokia5110_OutChar(49);
    Nokia5110_OutString("^1");
    Nokia5110_SetCursor(24,5);
    Nokia5110_OutString("^24h");
}

void incrementaRelogio()
{
    segundo_0++;

    if (segundo_0 > 57)
    {
        segundo_1++;
        segundo_0 = 48;

        if (segundo_1 >= 54)
        {
            minuto_0++;
            segundo_1 = 48;
        }
    }

    if (minuto_0 > 57)
    {
        minuto_1++;
        minuto_0 = 48;

        if (minuto_1 >= 54)
        {
            hora_0++;
            minuto_1 = 48;
        }
    }

    if (hora_0 > 57)
    {
        hora_0 = 48;
        hora_1++;
    }

    if (hora_0 == 52 && hora_1 == 50)
    {
        hora_0 = 48;
        hora_1 = 48;
    }
}


void imprimeRelogioDigital()
{
    char nova_hora[9] = {hora_1, hora_0, 58, minuto_1, minuto_0, 58,segundo_1, segundo_0,0};
    Nokia5110_Clear();
    Nokia5110_SetCursor(18,2);
    Nokia5110_OutString(nova_hora);
}


void SystickIntHandler (void)
{
    x++;
    y++;
    //cada 3 acessos ao systick ~ 1s
    //y==30 ~= 10s
    if (y > 30 && x==3){
        imprimeRelogioGrafico();
        incrementaRelogio();
        if (y>60) y=0;
        x=0;
    }
    if (x==3)
    {
        imprimeRelogioDigital();
        incrementaRelogio();
        x=0;
    }
}


void zeraRelogio()
{
    segundo_0 = 48;
    segundo_1 = 48;

    minuto_0 = 48;
    minuto_1 = 48;

    hora_0 = 48;
    hora_1 = 48;
}


void habilitaEntradaGPIO()
{
    //configura a porta pf0(sw1) e leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4,GPIO_FALLING_EDGE);
    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);
    IntMasterEnable();
}


void intPortalF(void)
{
    ui8PinData = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);

    if (ui8PinData == 0x10)//sw2
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xFF); //led azul
        zeraRelogio();
    }else if(ui8PinData == 0x11){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0x00);
    }else if(ui8PinData == 0x01){//sw1
        if (y<30 && y!=0){
            y=30;
        }else if (y>30 && y!=30)
            y=0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF); //led vermelho
    }
}


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    Nokia5110_Init();
    Nokia5110_ClearBuffer();
    Nokia5110_Clear();

    habilitaUART();

    SysTickIntEnable();
    SysTickPeriodSet(16777216);
    SysTickEnable();

    habilitaEntradaGPIO();

    while(1)
    {

    }

}
