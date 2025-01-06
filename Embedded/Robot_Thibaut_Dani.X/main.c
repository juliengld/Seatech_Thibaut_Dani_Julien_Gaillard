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
        }//eux ils ont Ã©tÃ© mis a jour
    }
}

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char capteurs = 0;

    // Encodage des capteurs dans un mot binaire
    if (robotState.distanceTelemetreExtremeGauche < 30) capteurs |= 0b10000; // Bit 4
    if (robotState.distanceTelemetreGauche < 30) capteurs |= 0b01000; // Bit 3
    if (robotState.distanceTelemetreCentre < 30) capteurs |= 0b00100; // Bit 2
    if (robotState.distanceTelemetreDroit < 30) capteurs |= 0b00010; // Bit 1
    if (robotState.distanceTelemetreExtremeDroit < 30) capteurs |= 0b00001; // Bit 0

    // Gestion des Ã©tats en fonction des capteurs
    switch (capteurs) {
            // Aucun obstacle
        case 0b00000:
            nextStateRobot = STATE_AVANCE;
            break;

            // Obstacle au centre uniquement
        case 0b00100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE;
            break;

            // Obstacles simultanÃ©s Ã   l'Extreme gauche et Ã  l'Extreme droite
        case 0b10001:
            nextStateRobot = STATE_AVANCE_PRUDENT;
            break;

        case 0b11010: // Cas extrÃªme gauche + droite + gauche
        case 0b10110: // Extreme Gauche + centre + droite
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11001: // Partout sauf droite et centre
        case 0b01000: // Gauche
            nextStateRobot = STATE_TOURNE_DROITE;
            break;

        case 0b01011: // Cas Extreme doite + droite + gauche 
        case 0b01101: // Extreme Droite + centre + gauche
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10011: //Partout sauf Gauche et Centre
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;
            

        case 0b10111: // Partout sauf gauche
        case 0b01010:// Obstacles simultanÃ©s Ã  gauche et Ã  droite
        case 0b10101: // Extremes + centre
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; // Tourne sur place vers la gauche
            break;

        case 0b11101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE; // Tourne sur place vers la gauche
            break;

            // Obstacles uniquement sur les extrÃªmes
        case 0b10000: // ExtrÃªme gauche uniquement
        case 0b10010: // Extreme gauche + droite
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;

        case 0b00001: // ExtrÃªme droite uniquement
        case 0b01001: // Extreme droite + gauche
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; // ExtrÃªme droite : virage lÃ©ger Ã  gauche
            break;

            // Obstacle Ã  l'extrÃªme gauche + gauche
        case 0b11000:
            nextStateRobot = STATE_TOURNE_DROITE; // Tourne Ã  droite normalement
            break;

            // Obstacle Ã  l'extrÃªme droite + droite
        case 0b00011:
            nextStateRobot = STATE_TOURNE_GAUCHE; // Tourne Ã  gauche normalement
            break;

            // Obstacles multiples Ã  gauche, incluant le centre
        case 0b10100:
        case 0b11100:
        case 0b01100: // Gauche + Centre
            nextStateRobot = STATE_TOURNE_DROITE;
            break;

            // Obstacles multiples Ã  droite, incluant le centre
        case 0b00110://Droite + Centre
        case 0b00111: // Partout sauf cotÃ© gauhce
        case 0b00010: // Droite
        case 0b00101: // Centre + Droite
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;

            // Obstacles partout sauf Ã  l'extrÃªme gauche
        case 0b01111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; // Tourner vers la droite (espace libre Ã  gauche)
            break;

            // Obstacles partout sauf Ã  l'extrÃªme droite
        case 0b11110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE; // Tourner vers la gauche (espace libre Ã  droite)
            break;


        case 0b11111: // Obstacles partout
        case 0b01110: // Obstacles partout sauf aux extremes
        case 0b11011: // Obstacles partout sauf au centre
            nextStateRobot = STATE_TOURNE_SUR_PLACE;
            break;

            // Gestion gÃ©nÃ©rique (prioritÃ© au centre, puis gauche, puis droite)
        default:
            if (capteurs & 0b00100) {
                nextStateRobot = STATE_TOURNE_SUR_PLACE; // Centre prioritaire
            } else if (capteurs & 0b11000) {
                nextStateRobot = STATE_TOURNE_DROITE; // Virage Ã  droite
            } else if (capteurs & 0b00011) {
                nextStateRobot = STATE_TOURNE_GAUCHE; // Virage Ã  gauche
            } else {
                nextStateRobot = STATE_STOP; // SÃ©curitÃ© par dÃ©faut
            }
            break;
    }

    //Si l’on n’est pas dans la transition de lé’tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;

}


//operatin system loop

void OperatingSystemLoop(void) {
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
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;

        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(-5, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(-30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(5, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE_LEGER:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_LEGER_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_LEGER_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE_LEGER:
            PWMSetSpeedConsigne(-30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_LEGER_EN_COURS;
            break;

        case STATE_TOURNE_GAUCHE_LEGER_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_AVANCE_PRUDENT:
            PWMSetSpeedConsigne(-10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_PRUDENT_EN_COURS;
            break;

        case STATE_AVANCE_PRUDENT_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_RECULE:
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-10, MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE_EN_COURS;
            break;

        case STATE_RECULE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}



