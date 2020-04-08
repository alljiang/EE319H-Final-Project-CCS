
#include "Controller.h"
#include <xdc/runtime/System.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"

double joystick1_h = 0;
double joystick1_v = 0;
bool btn1_a;
bool btn1_b;
bool btn1_start;
bool btn1_l;
bool btn1_r;

double joystick2_h = 0;
double joystick2_v = 0;
bool btn2_a;
bool btn2_b;
bool btn2_start;
bool btn2_l;
bool btn2_r;


void Controller_init() {
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0);  //  A1
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_1);  //  B1
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5);  //  START1
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);  //  L1
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_4);  //  R1

    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_5);  //  A2
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_4);  //  B2
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);  //  START2
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_3);  //  L2
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);  //  R2

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
}

void Controller_updateController() {
    btn1_a = !GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);  //  A1
    btn1_b = !GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);  //  B1
    btn1_start = !GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5);  //  START1
    btn1_l = !GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2);  //  L1
    btn1_r = !GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_4);  //  R1

    btn2_a = !GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_5);  //  A2
    btn2_b = !GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_4);  //  B2
    btn2_start = !GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6);  //  START2
    btn2_l = !GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3);  //  L2
    btn2_r = !GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0);  //  R2


    /*
     *  JS1_H: PD0, ADC CH7
     *  JS1_V: PD1, ADC CH6
     *  JS2_H: PD2, ADC CH5
     *  JS2_V: PD3, ADC CH4
     *  TRIG_L: PE1, ADC CH2
     *  TRIG_R: PE2, ADC CH1
     */

    uint32_t adcOut0=0, adcOut1=0;

    //  Joystick 1
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END
                             | ADC_CTL_CH7);
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);

    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END
                             | ADC_CTL_CH6);
    ADCSequenceEnable(ADC1_BASE, 0);
    ADCIntClear(ADC1_BASE, 0);

    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false)) {}
    ADCIntClear(ADC0_BASE, 0);
    ADCSequenceDataGet(ADC0_BASE, 0, &adcOut0);

    ADCProcessorTrigger(ADC1_BASE, 0);
    while(!ADCIntStatus(ADC1_BASE, 0, false)) {}
    ADCIntClear(ADC1_BASE, 0);
    ADCSequenceDataGet(ADC1_BASE, 0, &adcOut1);

//    System_printf("%d\t%d\n", adcOut0, adcOut1);

    if(adcOut0 >= 2500) joystick1_v = 1;
    else if(adcOut0 >= 90) joystick1_v = 0.3;
    else if(adcOut0 <= 10) joystick1_v = -1;
    else if(adcOut0 <= 70) joystick1_v = -0.3;
    else joystick1_v = 0;

    if(adcOut1 >= 4087) joystick1_h = -1;
    else if(adcOut1 >= 4020) joystick1_h = -0.3;
    else if(adcOut1 <= 2300) joystick1_h = 1;
    else if(adcOut1 <= 4000) joystick1_h = 0.3;
    else joystick1_h = 0;

    //  Joystick 2
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END
                             | ADC_CTL_CH5);
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);

    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END
                             | ADC_CTL_CH4);
    ADCSequenceEnable(ADC1_BASE, 0);
    ADCIntClear(ADC1_BASE, 0);

    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false)) {}
    ADCIntClear(ADC0_BASE, 0);
    ADCSequenceDataGet(ADC0_BASE, 0, &adcOut0);

    ADCProcessorTrigger(ADC1_BASE, 0);
    while(!ADCIntStatus(ADC1_BASE, 0, false)) {}
    ADCIntClear(ADC1_BASE, 0);
    ADCSequenceDataGet(ADC1_BASE, 0, &adcOut1);

    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceDisable(ADC1_BASE, 0);

//    System_printf("%d\t%d\n", adcOut0, adcOut1);

    if(adcOut0 >= 4095) joystick2_v = 1;
    else if(adcOut0 >= 1950) joystick2_v = 0.3;
    else if(adcOut0 <= 16) joystick2_v = -1;
    else if(adcOut0 <= 600) joystick2_v = -0.3;
    else joystick2_v = 0;

    if(adcOut1 >= 4095) joystick2_h = -1;
    else if(adcOut1 >= 2050) joystick2_h = -0.3;
    else if(adcOut1 <= 20) joystick2_h = 1;
    else if(adcOut1 <= 1890) joystick2_h = 0.3;
    else joystick2_h= 0;

    //  Joystick 1 Triggers
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END
                             | ADC_CTL_CH2);
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);

    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END
                             | ADC_CTL_CH1);
    ADCSequenceEnable(ADC1_BASE, 0);
    ADCIntClear(ADC1_BASE, 0);

    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false)) {}
    ADCIntClear(ADC0_BASE, 0);
    ADCSequenceDataGet(ADC0_BASE, 0, &adcOut0);

    ADCProcessorTrigger(ADC1_BASE, 0);
    while(!ADCIntStatus(ADC1_BASE, 0, false)) {}
    ADCIntClear(ADC1_BASE, 0);
    ADCSequenceDataGet(ADC1_BASE, 0, &adcOut1);

    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceDisable(ADC1_BASE, 0);

    btn1_l = adcOut0 < 600;
    btn1_r = adcOut1 > 600;

//    System_printf("%d\t%d\n\n", adcOut0, adcOut1);

//    System_printf("%d\t%d\n%d\t%d\n%d\t%d\n\n", (int)(joystick1_h*10), (int)(joystick1_v*10), (int)(joystick2_h*10), (int)(joystick2_v*10), btn1_l, btn1_r);

    System_flush();
}

double getJoystick_h(uint8_t player) {
    if(player == 2) return joystick2_h;
    else return joystick1_h;
}

double getJoystick_v(uint8_t player) {
    if(player == 2) return joystick2_v;
    else return joystick1_v;
}

bool getBtn_a(uint8_t player) {
    if(player == 2) return btn2_a;
    else return btn1_a;
}

bool getBtn_b(uint8_t player) {
    if(player == 2) return btn2_b;
    else return btn1_b;
}

bool getBtn_l(uint8_t player) {
    if(player == 2) return btn2_l;
    else return btn1_l;
}

bool getBtn_r(uint8_t player) {
    if(player == 2) return btn2_r;
    else return btn1_r;
}

bool getBtn_start(uint8_t player) {
    if(player == 2) return btn2_start;
    else return btn1_start;
}
