#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"

//unsigned int ADCValue0, ADCValue1, ADCValue2;
    
int main(void) {
    /***************************************************************************/
    //Initialisation oscillateur
    /***************************************************************************/
    InitOscillator();
    /******************************************************************************/
    // Configuration des input et output (IO)
    /******************************************************************************/

    InitIO();
    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;
    
   
    
   
    InitPWM();
    InitADC1();
    InitTimer23();
    InitTimer1();
    
    /******************************************************************************/
    // Boucle Principale
    /******************************************************************************/
    while (1) {
        float distance30 = 0x936;
        if (ADCIsConversionFinished() == 1)
{
        ADCClearConversionFinishedFlag();
        unsigned int * result = ADCGetResult();
        
        float volts = ((float) result [0])* 3.3 / 4096;        
        robotState.distanceTelemetreDroit = 34 / volts - 5;
        volts = ((float) result [1])* 3.3 / 4096;
        robotState.distanceTelemetreCentre = 34 / volts - 5;
        volts = ((float) result [2])* 3.3 / 4096;
        robotState.distanceTelemetreGauche = 34 / volts - 5;
        
        if(result[0] > distance30){
           LED_BLEUE_2 = 1;
        }else{
           LED_BLEUE_2 = 0;
        }
         if(result[1] > distance30){
            LED_ORANGE_2 = 1;
        }else{
             LED_ORANGE_2 = 0;
        }
         if(result[2] > distance30){
            LED_ROUGE_2 = 1;
        }else{
             LED_ROUGE_2 = 0;
        }
        
}
            
            
        }
    } // fin main