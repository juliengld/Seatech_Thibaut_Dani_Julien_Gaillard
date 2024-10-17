#ifndef PWM_H
#define PWM_H
#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1


void InitPWM();
void PWMSetSpeed(float vitesseEnPourcents, int numeroMoteur);
void PWMSetSpeedConsigne (int vitesseEnPourcents, int numeroMoteur);

#endif
