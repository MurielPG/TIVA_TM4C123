#include "minhaLib.h"
#include "artes.h"

uint8_t ui8PinData=0;

int vez = 0;
int contadorSeta = 0;
char tabela[3][3];

int inicial=0;


int imprimeSimbolo(int x, int y, char simb)
{
    char vencedor;

    if((x>2) || (y>2)) return 0;
    if((x<0) || (y<0)) return 0;

    int ciruclo_x[] = {27,28,29,26,30,25,31,25,31,25,31,26,30,27,28,29};
    int ciruclo_y[] = {7 ,7 ,7 ,8 ,8 ,9 ,9 ,10,10,11,11,12,12,13,13,13};

    int cruz_x[] = {26,30,27,29,28,27,29,26,30};
    int cruz_y[] = {8 ,8 ,9 ,9 ,10,11,11,12,12};

    int i=0;

    if (simb == 'x' || simb == 'X')
    {
        if ((tabela[x][y] == 'x') || (tabela[x][y] == 'o'))
            return 0;
        else{
            for(i=0;i<9;i++)
                Nokia5110_SetPxl( (cruz_y[i]+14*y), (cruz_x[i]+14*x) );
            tabela[x][y] = 'x';
            trocaVez();
        }
    }
    if (simb == 'o' || simb == 'O')
    {
        if ((tabela[x][y] == 'x') || (tabela[x][y] == 'o'))
            return 0;
        else{
            for(i=0;i<16;i++)
                Nokia5110_SetPxl( (ciruclo_y[i]+14*y), (ciruclo_x[i]+14*x) );
            tabela[x][y] = 'o';
            trocaVez();
        }
    }

    Nokia5110_DisplayBuffer();

    vencedor = testaVencedor();
    if (vencedor=='x'){
        Nokia5110_DrawFullImage(x_vencedor);
        SysCtlDelay(SysCtlClockGet());
        reiniciaJogo();
    }
    else if (vencedor=='o'){
        Nokia5110_DrawFullImage(o_vencedor);
        SysCtlDelay(SysCtlClockGet());
        reiniciaJogo();
    }

    return 0;
}

char testaVencedor()
{
    int i=0;

    for(i=0; i<3;i++){
        if (tabela[i][0] == tabela[i][1])
            if (tabela[i][0] == tabela[i][2])
                return tabela[i][0];
    }

    for(i=0; i<3;i++){
        if (tabela[0][i] == tabela[1][i])
            if (tabela[0][i] == tabela[2][i])
                return tabela[0][i];
    }


    if (tabela[0][0] == tabela[1][1])
        if (tabela[1][1] == tabela[2][2])
            return tabela[0][0];

    if (tabela[0][2] == tabela[1][1])
        if (tabela[1][1] == tabela[2][0])
            return tabela[0][2];


    return 's';
}

void trocaVez()
{
    if (vez == 0)
        vez = 1;
    else
        vez = 0;
}

void IntPortalE(void)
{
    SysCtlDelay((SysCtlClockGet()/3)/10);
    ui8PinData = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);

    if(ui8PinData==B2){
        if (vez == 0)
            imprimeSimbolo(0, contadorSeta  ,'o');
        else
            imprimeSimbolo(0, contadorSeta  ,'x');
    }
    if(ui8PinData==B3){
        if (vez == 0)
            imprimeSimbolo(1, contadorSeta  ,'o');
        else
            imprimeSimbolo(1, contadorSeta  ,'x');
    }
    if(ui8PinData==B4){
        if (vez == 0)
            imprimeSimbolo(2, contadorSeta  ,'o');
        else
            imprimeSimbolo(2, contadorSeta  ,'x');
    }
    if(ui8PinData==B1){
        contadorSeta++;
        if (contadorSeta>2) contadorSeta=0;
        imprimeSeta( (16*contadorSeta) );
    }
}

void intPortalF(void)
{
    ui8PinData = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);

    if (ui8PinData == B5)
    {
        inicial=2;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xFF);
        Nokia5110_Clear();
        Nokia5110_SetCursor(9,2);
        Nokia5110_OutString("Prepare-se");
        SysCtlDelay(SysCtlClockGet()/3);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
        imprimeTabela();

    }if(ui8PinData == B6){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xFF);
        Nokia5110_Clear();
        Nokia5110_OutString("Primeiro botao seleciona     a linha");
        SysCtlDelay(SysCtlClockGet());
        Nokia5110_Clear();
        Nokia5110_OutString("e os tres seguintes para escolher quadrante");
        SysCtlDelay(SysCtlClockGet());
        Nokia5110_Clear();
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
        imprimeTabela();
    }
}

void reiniciaJogo()
{
    int i=0, j=0;

    for (i=0;i<3;i++){
        for (j=0;j<3;j++){
            tabela[i][j]='s';
        }
    }

    Nokia5110_Clear();
    Nokia5110_ClearBuffer();
    imprimeTabela();

}

void configuraGPIO()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);

    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    IntEnable(INT_GPIOE);
    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);

    IntMasterEnable();
}

void imprimeTabela()
{
    int i=0;
    for(i=2; i<46; i++)//x
    {
        Nokia5110_SetPxl(i,34);
        Nokia5110_SetPxl(i,50);

        Nokia5110_SetPxl(i,35);
        Nokia5110_SetPxl(i,49);
    }

    for(i=18; i<66; i++)//y
    {
        Nokia5110_SetPxl(16,i);
        Nokia5110_SetPxl(15,i);

        Nokia5110_SetPxl(32,i);
        Nokia5110_SetPxl(31,i);
    }


    imprimeSeta( (16*contadorSeta) );
    Nokia5110_DisplayBuffer();
}

void imprimeSeta(int posicao)
{
    int eixo_y[]={5,6,7,8,9,6,7,8,7};
    int eixo_x[]={8,8,8,8,8,9,9,9,10};
    int i=0;

    for(i=0;i<9;i++)
    {
        Nokia5110_ClrPxl((eixo_y[i] + 0), eixo_x[i]);
        Nokia5110_ClrPxl((eixo_y[i] + 16), eixo_x[i]);
        Nokia5110_ClrPxl((eixo_y[i] + 32), eixo_x[i]);
    }

    for(i=0;i<9;i++)
    {
        Nokia5110_SetPxl((posicao + eixo_y[i]), eixo_x[i]);
    }

    Nokia5110_DisplayBuffer();
}

void carregaTelaInical()
{
    Nokia5110_DrawFullImage(tela_inicial);
    uint32_t delay = SysCtlClockGet()/3;;
    while (delay>0 && inicial==0)
        delay--;
    Nokia5110_Clear();
    imprimeTabela();
}



