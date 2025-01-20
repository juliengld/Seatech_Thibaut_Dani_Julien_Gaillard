
#include <xc.h>
#include "IO.h"

void InitIO() {
    //****************************************************************************************************/
    // Declaration des pin Analogiques
    //****************************************************************************************************/
    ANSELA = 0; //Desactivation de toutes entree analogique
    ANSELB = 0; //Desactivation de toutes entree analogique
    ANSELD = 0; //Desactivation de toutes entree analogique
    ANSELC = 0; //Desactivation de toutes entree analogique
    ANSELE = 0; //Desactivation de toutes entree analogique
    ANSELG = 0; //Desactivation de toutes entree analogique

    // Configuration des sorties

    //******* LED ***************************
    _TRISJ6 = 0; // LED Orange_1
    _TRISJ5 = 0; //LED Blanche_1
    _TRISJ4 = 0; // LED Bleue_1
    _TRISJ11 = 0; // LED Rouge_1
    _TRISH10 = 0; // LED Verte _1
    
    _TRISA9 = 0; //LED_Bleue_2
    _TRISA10= 0; //LED_Rouge_2
    _TRISH3 = 0; //LED_Verte_2
    _TRISK15 = 0; //LED_Orange_2
    _TRISA0 = 0; //LED_Blanche_2
    
    _TRISH1 = 0; //Bouton 
    
    
    
    
    
    

    //****** Moteurs ************************

    // Configuration des entrées


    /****************************************************************************************************/
    // Gestion des pin remappables
    /****************************************************************************************************/
    UnlockIO(); // On unlock les registres d'entrées/sorties, ainsi que les registres des PPS

    //Assignation des remappable pins
    _U1RXR = 78;
    _RP79R = 0b00001; //Remappe la sortie Tx1 vers RP...


    LockIO(); // On lock les registres d'entrées/sorties, ainsi que les registres des PPS
}

void LockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bset OSCCON, #6":: : "w1", "w2", "w3");
}

void UnlockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bclr OSCCON, #6":: : "w1", "w2", "w3");
}
