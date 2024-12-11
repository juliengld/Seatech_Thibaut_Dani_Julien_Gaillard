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
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreCentreDroit = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
        }//eux ils ont été mis a jour
    }
}

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //éDtermination de la position des obstacles en fonction des ééètlmtres
    if (robotState.distanceTelemetreCentreDroit < 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreCentreGauche > 30) //Obstacle àdroite
        positionObstacle = OBSTACLE_UN_PEU_A_DROITE;
    else if (robotState.distanceTelemetreCentreDroit < 30 &&
            robotState.distanceTelemetreCentre < 20 &&
            robotState.distanceTelemetreCentreGauche < 30)
        positionObstacle = OBSTACLE_EN_FACE_DROIT;
    else if (robotState.distanceTelemetreCentreDroit > 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreCentreGauche < 30) //Obstacle un peu àgauche
        positionObstacle = OBSTACLE_UN_PEU_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < 20 &&
            robotState.distanceTelemetreCentreDroit < 30 &&
            robotState.distanceTelemetreDroit < 15 ) //Obstacle en face et a doite
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreCentre < 20 &&
            robotState.distanceTelemetreCentreGauche < 30 &&
            robotState.distanceTelemetreGauche < 15 ) //Obstacle en face et a gauche
        positionObstacle = OBSTACLE_EN_FACE_GAUCHE;
    else if (robotState.distanceTelemetreDroit > 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreGauche > 30) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
    else if (robotState.distanceTelemetreCentreGauche < 30 &&
            robotState.distanceTelemetreGauche < 15)
        positionObstacle = OBSTACLE_A_GAUCHE; //a créer!!!!!!!!!!!!!!!!!! 
    else if (robotState.distanceTelemetreCentreDroit < 30 &&
            robotState.distanceTelemetreDroit < 15)
        positionObstacle = OBSTACLE_A_DROITE;
            
            
    //éDtermination de lé?tat àvenir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_RECULE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE_DROIT)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    else if (positionObstacle == OBSTACLE_EN_FACE_GAUCHE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    else if (positionObstacle == OBSTACLE_A_DROITE) //de la a la faut modif 
        nextStateRobot = STATE_TOURNE_FORT_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_FORT_DROIT;              //à la en fait 
    //Si l?on n?est pas dans la transition de lé?tape en cours
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
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
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
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(7, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(7, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;  
        case STATE_RECULE:
            PWMSetSpeedConsigne(-18, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE_EN_COURS;
            break;
        case STATE_RECULE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;  
        case STATE_TOURNE_FORT_DROIT:
            PWMSetSpeedConsigne(7, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_FORT_DROIT_EN_COURS;
            break;
        case STATE_TOURNE_FORT_GAUCHE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(7, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_FORT_GAUCHE_EN_COURS;
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
            
    }
}



