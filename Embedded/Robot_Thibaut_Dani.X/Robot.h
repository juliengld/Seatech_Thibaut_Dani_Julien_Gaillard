#ifndef ROBOT_H
#define ROBOT_H

typedef struct robotStateBITS {

    union {

        struct {
            unsigned char taskEnCours;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;
            float acceleration;
            float distanceTelemetreGauche;
            float distanceTelemetreDroit;
            float distanceTelemetreCentre;
            float distanceTelemetreExtremeGauche;
            float distanceTelemetreExtremeDroit;
        };
    };
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState;
#endif /* ROBOT_H */

