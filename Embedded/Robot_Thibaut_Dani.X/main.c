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
#include "UART.h"
#include "CB_TX1.h"

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
    
    InitUART();
    CB_TX1_Init();
    CB_RX1_Init();
    


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
        }
        
        //unsigned char message[] = "Etat du robot: En fonctionnement\n";
        //int messageLength = sizeof(message) - 1; // Taille du message (sans le caractère nul)
        //SendMessage(message, messageLength);

        // Délai pour éviter d'envoyer le message en boucle trop rapidement
        //__delay32(40000000); // Délai de 1 seconde (à ajuster selon la fréquence du CPU)
        
        int i;
        for(i=0; i< CB_RX1_GetDataSize(); i++)
        {
            unsigned char c = CB_RX1_Get();
            SendMessage(&c,1);
        }
        __delay32(10000);

    }
}

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char capteurs = 0;

    // Encodage des capteurs dans un mot binaire
    if (robotState.distanceTelemetreExtremeGauche < 40) capteurs |= 0b10000; // Bit 4
    if (robotState.distanceTelemetreGauche < 35) capteurs |= 0b01000; // Bit 3
    if (robotState.distanceTelemetreCentre < 35) capteurs |= 0b00100; // Bit 2
    if (robotState.distanceTelemetreDroit < 35) capteurs |= 0b00010; // Bit 1
    if (robotState.distanceTelemetreExtremeDroit < 40) capteurs |= 0b00001; // Bit 0

   
    // Gestion des ÃÂÃÂ©tats en fonction des capteurs
    switch (capteurs) { 
        
    case 0b00000: nextStateRobot = STATE_AVANCE;
        break;
    case 0b00001: nextStateRobot = STATE_TOURNE_GAUCHE;
        break;
    case 0b00010: nextStateRobot = STATE_TOURNE_GAUCHE;
        break;
    case 0b00011: nextStateRobot = STATE_TOURNE_GAUCHE;
        break;
    case 0b00100: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b00101: nextStateRobot = STATE_TOURNE_GAUCHE;
        break;
    case 0b00110: nextStateRobot = STATE_TOURNE_GAUCHE;
        break;
    case 0b00111: nextStateRobot = STATE_TOURNE_GAUCHE;
        break;
    case 0b01000: nextStateRobot = STATE_TOURNE_DROITE;
        break;
    case 0b01010: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b01011: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b01100: nextStateRobot = STATE_TOURNE_DROITE;
        break;
    case 0b01101: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b01110: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b01111: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b10000: nextStateRobot = STATE_TOURNE_DROITE;
        break;
    case 0b10001: nextStateRobot = STATE_AVANCE;
        break;
    case 0b10010: nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        break;
    case 0b10011: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b10100: nextStateRobot = STATE_TOURNE_DROITE;
        break;
    case 0b10101: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b10110: nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        break;
    case 0b10111: nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        break;
    case 0b11000: nextStateRobot = STATE_TOURNE_DROITE;
        break;
    case 0b11001: nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;//MOdif
        break;
    case 0b11010: nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        break;
    case 0b11011: nextStateRobot = STATE_TOURNE_SUR_PLACE;
        break;
    case 0b11100: nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        break;
    case 0b11101: nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        break;
    case 0b11110: nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        break;
    case 0b11111: nextStateRobot = STATE_TOURNE_SUR_PLACE;
        break;
    default: nextStateRobot = STATE_AVANCE;
        break;
            
    }

    //Si lÃ¢ÂÂon nÃ¢ÂÂest pas dans la transition de lÃÂ©Ã¢ÂÂtape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;

}


//operatin system loop

void OperatingSystemLoop(void) {
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
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_EN_COURS;
            break;
            
            

        case STATE_TOURNE_SUR_PLACE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;


        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(-7, MOTEUR_DROIT);
            PWMSetSpeedConsigne(17, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(17, MOTEUR_DROIT);//20 sinon
            PWMSetSpeedConsigne(-7, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
            

        default:
            
            stateRobot = STATE_AVANCE;
            break;
    }
}
}
