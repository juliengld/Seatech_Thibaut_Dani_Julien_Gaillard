#ifndef CB_RX1_H
#define CB_RX1_H

#include <xc.h>
#include <stdint.h>

// Taille du buffer circulaire
#define CBRX1_BUFFER_SIZE 128

// Déclaration des variables globales
extern int cbRx1Head; // Indice d'insertion (écriture)
extern int cbRx1Tail; // Indice de lecture
extern unsigned char cbRx1Buffer[CBRX1_BUFFER_SIZE]; // Buffer circulaire

// Prototypes des fonctions
void CB_RX1_Add(unsigned char value);
unsigned char CB_RX1_Get(void);
unsigned char CB_RX1_IsDataAvailable(void);
int CB_RX1_GetDataSize(void);
int CB_RX1_GetRemainingSize(void);

#endif // CB_RX1_H