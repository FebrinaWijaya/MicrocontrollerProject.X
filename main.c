#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
// using namespace std;
void adc_init(void);
void ccp2_init(void);
void tmr_init(void);
int array[8];
int i=0;

void __interrupt(high_priority) Hi_ISR(void)
{
    //deal ccp2 interrupt and adc interrupt
    if(PIR1bits.ADIF){//AD conversion finish
	//set GO/DONE ADCON0
	//check GO/DONE(clear is setting finish)
	//result in ADRESH:ADRESL

        PIR1bits.ADIF = 0;
        ADCON0bits.ADON = 1;//on
        //NOP();
        RCREG = ADRES;
        array[i] = ADRES;
//        if(array[i]<400) LATD = 1;
//        else LATD = 0;
        
        if(i <  7){
            i = i+1;
        }else{
            i = 0;
        } 
    }
    else if(PIR2bits.CCP2IF){ //special event triGger
        PIR2bits.CCP2IF = 0;
        CCPR2 = 31250;  // 1M/4/8hz
        ADCON0bits.GO = 1;
        //TMR3 = 0;
    }
    return ;
}

void main(void) 
{  
    SYSTEM_Initialize() ;
    
    TRISD = 0;
    adc_init();
    ccp2_init();
    tmr_init();
    TXREG = 1; //connect RX and TX pin by cable
    
    
    while(1);
    return;
}

void adc_init(void){
    //datasheet p232 TABLE 19-2
    //Configure the A/D module
    //ADCON0
    //select analog channel
    //set TRIS
    //Turn on A/D module
    //ADCON1 //set refer voltage

    //ADCON2
    //A/D Conversion Clock
    //Acquisition Time
    //left or right justified
    //Configure A/D interrupt(optional)
    //enable Interrupt Priority mode
    ADCON1bits.VCFG1 = 0; //setting vref- = Vss
    ADCON1bits.VCFG0 = 0; //setting vref+ = Vdd
    ADCON1bits.PCFG = 0; //setting A/D port configuration control(AN0~AN12 = analog)
    ADCON0bits.CHS = 7; //setting input channel = AN7
    TRISEbits.TRISE2 = 1; //setting RE2/AN7 port as input
    //TRISE =  0x02   ;
    ADCON2bits.ADFM = 1; //setting RIGHT justified

    ADCON2bits.ACQT = 1; //setting acquisition time ACQT<2:0> 2Tad = 2.4 us (Tacqt should be >=2.4us)
    ADCON2bits.ADCS = 3; //conversion time Frc (Tad = 1.2 us)
    ADCON0bits.ADON = 1; // enable AD module

    PIR1bits.ADIF = 0;//clear ADIF
    PIE1bits.ADIE = 1; //set ADIE
    INTCONbits.GIE = 1;//set GIE
    IPEN = 1;
    ADIP = 1;

}

void ccp2_init(void){
    //Configure CCP2 mode,ref datasheet p139
    CCP2M3 = 1;
    CCP2M2 = 0;
    CCP2M1 = 1;
    CCP2M0 = 1;
    //configure CCP2 interrupt
    CCP2IE = 1;
    CCP2IP = 1;
    //configure CCP2 comparator value
    CCPR2 = 31250; // 1M/4/8hz
}

void tmr_init(void){
    //set up timer3, ref datasheet p135
    T3CCP1 = 1;
    T3CCP2 = 0;
    TMR3ON = 1;
    T3CONbits.RD16 = 1; //16-bit timer
    //no need to turn up timer3 interrupt
}
