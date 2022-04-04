#include "minhaLib.h"

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    Nokia5110_Init();
    Nokia5110_ClearBuffer();
    Nokia5110_Clear();

    configuraGPIO();
    carregaTelaInical();

    imprimeTabela();

    while(1)
    {

    }
}
