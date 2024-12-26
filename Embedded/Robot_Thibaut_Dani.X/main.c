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
    InitTimer1();
    //InitTimer23();
    InitTimer4();


    //robotState.acceleration=1;

    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;

    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 0;
    LED_VERTE_2 = 1;

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
        }//eux ils ont été mis a jour
    }
}

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char capteurs = 0;

    // Encodage des capteurs dans un mot binaire
    if (robotState.distanceTelemetreExtremeGauche < 30) capteurs |= 0b10000; // Bit 4
    if (robotState.distanceTelemetreGauche < 30) capteurs |= 0b01000;        // Bit 3
    if (robotState.distanceTelemetreCentre < 20) capteurs |= 0b00100;        // Bit 2
    if (robotState.distanceTelemetreDroit < 30) capteurs |= 0b00010;         // Bit 1
    if (robotState.distanceTelemetreExtremeDroit < 30) capteurs |= 0b00001;  // Bit 0

    // Gestion des états en fonction des capteurs
    switch (capteurs) {
        // Aucun obstacle
        case 0b00000:
            nextStateRobot = STATE_AVANCE;
            break;

        // Obstacle au centre uniquement
        case 0b00100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE;
            break;

        // Obstacles simultanés à  l'Extreme gauche et à l'Extreme droite
        case 0b10001:
            nextStateRobot = STATE_AVANCE_PRUDENT
            break;
        
        case 0b11010: // Cas extrême gauche + droite + gauche
        case 0b11001: // Partout sauf droite et centre
        case 0b10110: // Extreme Gauche + centre + droite
        case 0b01000: // Gauche
            nextStateRobot = STATE_TOURNE_DROITE
            break;
        
        case 0b01011: // Cas Extreme doite + droite + gauche 
        case 0b10011: //Partout sauf Gauche et Centre
        case 0b01101: // Extreme Droite + centre + gauche
            nextStateRobot = STATE_TOURNE_GAUCHE
            break;
        
        case 0b10111: // Partout sauf gauche
        case 0b01010:// Obstacles simultanés à gauche et à droite
        case 0b10101: // Extremes + centre
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; // Tourne sur place vers la gauche
            break;

        case 0b11101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE; // Tourne sur place vers la gauche
            break;

        // Obstacles uniquement sur les extrêmes
        case 0b10000: // Extrême gauche uniquement
        case 0b10010: // Extreme gauche + droite
            nextStateRobot = STATE_TOURNE_DROITE_LEGER; // Extrême gauche : virage léger à droite
            break;

        case 0b00001: // Extrême droite uniquement
        case 0b01001: // Extreme droite + gauche
            nextStateRobot = STATE_TOURNE_GAUCHE_LEGER; // Extrême droite : virage léger à gauche
            break;

        // Obstacle à l'extrême gauche + gauche
        case 0b11000:
            nextStateRobot = STATE_TOURNE_DROITE; // Tourne à droite normalement
            break;

        // Obstacle à l'extrême droite + droite
        case 0b00011:
            nextStateRobot = STATE_TOURNE_GAUCHE; // Tourne à gauche normalement
            break;

        // Obstacles multiples à gauche, incluant le centre
        case 0b10100:
        case 0b11100:
        case 0b01100: // Gauche + Centre
            nextStateRobot = STATE_TOURNE_DROITE;
            break;

        // Obstacles multiples à droite, incluant le centre
        case 0b00110://Droite + Centre
        case 0b00111: // Partout sauf coté gauhce
        case 0b00010: // Droite
        case 0b00101 // Centre + Droite
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;

        // Obstacles partout sauf à l'extrême gauche
        case 0b01111:
            nextStateRobot = STATE_TOURNE_GAUCHE; // Tourner vers la droite (espace libre à gauche)
            break;

        // Obstacles partout sauf à l'extrême droite
        case 0b11110:
            nextStateRobot = STATE_TOURNE_DROITE; // Tourner vers la gauche (espace libre à droite)
            break;

        
        case 0b11111: // Obstacles partout
        case 0b01110: // Obstacles partout sauf aux extremes
        case 0b11011: // Obstacles partout sauf au centre
            nextStateRobot = STATE_TOURNE_SUR_PLACE;
            break;

        // Gestion générique (priorité au centre, puis gauche, puis droite)
        default:
            if (capteurs & 0b00100) {
                nextStateRobot = STATE_TOURNE_SUR_PLACE; // Centre prioritaire
            } else if (capteurs & 0b11000) {
                nextStateRobot = STATE_TOURNE_DROITE; // Virage à droite
            } else if (capteurs & 0b00011) {
                nextStateRobot = STATE_TOURNE_GAUCHE; // Virage à gauche
            } else {
                nextStateRobot = STATE_STOP; // Sécurité par défaut
            }
            break;
    }
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
            PWMSetSpeedConsigne(-20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;

        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(-10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(-20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE_LEGER:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_LEGER_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_LEGER_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE_LEGER:
            PWMSetSpeedConsigne(-20, MOTEUR_DROIT);
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
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
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



