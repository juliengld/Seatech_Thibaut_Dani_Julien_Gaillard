#ifndef CB_TX1_H
#define CB_TX1_H

#include <xc.h>
#include <stdint.h>

// Taille du buffer circulaire
#define CBTX1_BUFFER_SIZE 128

// Déclaration des variables globales
extern int cbTx1Head; // Indice d'insertion (écriture)
extern int cbTx1Tail; // Indice de lecture
extern unsigned char cbTx1Buffer[CBTX1_BUFFER_SIZE]; // Buffer circulaire
extern unsigned char isTransmitting; // Indicateur de transmission en cours

// Prototypes des fonctions
void SendMessage(unsigned char* message, int length);
void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get(void);
void SendOne(void);
unsigned char CB_TX1_IsTransmitting(void);
int CB_TX1_GetDataSize(void);
int CB_TX1_GetRemainingSize(void);

#endif // CB_TX1_H