#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "robot.h"
#include "main.h"

int main(void) {

    /**************************************************************************/
    //Initialisation oscillateur
    /**************************************************************************/
    InitOscillator();
    /***********************************************************************************************/
    // Configuration des input et output (IO)
    /***********************************************************************************************/
    InitIO();
    /**************************************************************************/
    //Initialisation PWM
    /**************************************************************************/
    InitPWM();
    /**************************************************************************/
    //Initialisation ADC
    /**************************************************************************/
    InitADC1();
    /**************************************************************************/
    //Initialisation timer
    /**************************************************************************/
    //InitTimer1();
    //InitTimer23();
    InitTimer4();


    //robotState.acceleration=1;

    /***********************************************************************************************/
    // Boucle Principale
    /***********************************************************************************************/
    while (1) {

        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreExtremeDroit = 34 / volts - 5;

            if (robotState.distanceTelemetreCentre < 30)
                LED_ORANGE_1 = 1;
            else
                LED_ORANGE_1 = 0;
             if (robotState.distanceTelemetreExtremeGauche < 30)
                LED_BLANCHE_1 = 1;
            else
                LED_BLANCHE_1 = 0;
             if (robotState.distanceTelemetreGauche < 30)
                LED_BLEUE_1 = 1;
            else
                LED_BLEUE_1 = 0;
             if (robotState.distanceTelemetreDroit < 30)
                LED_ROUGE_1 = 1;
            else
                LED_ROUGE_1 = 0;
             if (robotState.distanceTelemetreExtremeDroit < 30)
                LED_VERTE_1 = 1;
            else
                LED_VERTE_1 = 0;
        }//eux ils ont ÃÂÃÂ©tÃÂÃÂ© mis a jour
    }
}

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char capteurs = 0;

    // Encodage des capteurs dans un mot binaire
    if (robotState.distanceTelemetreExtremeGauche < 35) capteurs |= 0b10000; // Bit 4
    if (robotState.distanceTelemetreGauche < 35) capteurs |= 0b01000; // Bit 3
    if (robotState.distanceTelemetreCentre < 35) capteurs |= 0b00100; // Bit 2
    if (robotState.distanceTelemetreDroit < 35) capteurs |= 0b00010; // Bit 1
    if (robotState.distanceTelemetreExtremeDroit < 35) capteurs |= 0b00001; // Bit 0

   
    // Gestion des ÃÂÃÂ©tats en fonction des capteurs
    switch (capteurs) {
            // Aucun obstacle
        case 0b00000:
        case 0b10001:
            nextStateRobot = STATE_AVANCE;
            break;
        
        
        //Obstacle par défaut
        case 0b00100:
        case 0b01110:
        case 0b01010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        //Obstacle dangereux
        case 0b10101:
        case 0b11111:
        case 0b11011:
        case 0b10111:
        case 0b11101:
        case 0b10110:
        case 0b01101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE;
            break;
            
        //Obstacle à droite majortiaire
        case 0b00111:
        case 0b10011:
        case 0b10010:
        case 0b01111:
        case 0b01011:
            nextStateRobot= STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b01001:
        case 0b11100:
        case 0b11010:
        case 0b11110:
        case 0b11001:
            nextStateRobot= STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            

        case 0b10000:
        case 0b01000:
        case 0b11000:
        case 0b10100:
        case 0b01100: 
            nextStateRobot = STATE_TOURNE_DROITE;
            break;
         
        case 0b00010:
        case 0b00001:
        case 0b00011:
        case 0b00101:
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;
            
        default :
            nextStateRobot = STATE_AVANCE;
            break;
            
    }

    //Si lÃ¢ÂÂon nÃ¢ÂÂest pas dans la transition de lÃÂ©Ã¢ÂÂtape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;

}


//operatin system loop

void OperatingSystemLoop(void) {
    unsigned int timeout = 0;
    if (timestamp>60000) {
        PWMSetSpeedConsigne(0, MOTEUR_DROIT);
        PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
    }
    else {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
            break;

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;

        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
            
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(7, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-7, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_EN_COURS;
            break;
            
        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-7, MOTEUR_DROIT);
            PWMSetSpeedConsigne(7, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_EN_COURS;
            break;
            
        case STATE_TOURNE_SUR_PLACE :
            PWMSetSpeedConsigne(-13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_EN_COURS;
            break;
            
            

        case STATE_TOURNE_SUR_PLACE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;


        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(5, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(5, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        default:
            timeout = 0;
            
            stateRobot = STATE_AVANCE;
            break;
    }
}
}
