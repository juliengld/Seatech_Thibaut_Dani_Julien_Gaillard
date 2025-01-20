#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_RX1.h"
#define CBRX1_BUFFER_SIZE 128
int cbRx1Head;
int cbRx1Tail;
unsigned char cbRx1Buffer[CBRX1_BUFFER_SIZE];

void CB_RX1_Init(void) {
    cbRx1Head = 0; // Initialiser l'indice d'insertion (écriture)
    cbRx1Tail = 0; // Initialiser l'indice de lecture

    // Optionnel : Nettoyer le buffer (mettre tous les éléments à 0)
    for (int i = 0; i < CBRX1_BUFFER_SIZE; i++) {
        cbRx1Buffer[i] = 0;
    }
}
void CB_RX1_Add(unsigned char value)
{
    if(CB_RX1_GetRemainingSize()>0)
        {
            cbRx1Buffer[cbRx1Head] = value; // Ajouter la valeur au buffer
            cbRx1Head = (cbRx1Head + 1) % CBRX1_BUFFER_SIZE; 
        }
}
unsigned char CB_RX1_Get(void)
{
        unsigned char value=cbRx1Buffer[cbRx1Tail];
        cbRx1Tail = (cbRx1Tail + 1) % CBRX1_BUFFER_SIZE; // Mettre à jour l'indice de lecture    
        return value;
}
unsigned char CB_RX1_IsDataAvailable(void)
{
    if(cbRx1Head!=cbRx1Tail)
        return 1;
    else
        return 0;
}
int CB_RX1_GetDataSize(void)
{
//return size of data stored in circular buffer
    int dataSize;

    if (cbRx1Head >= cbRx1Tail) {
        dataSize = cbRx1Head - cbRx1Tail;
    } else {
        dataSize = CBRX1_BUFFER_SIZE - (cbRx1Tail - cbRx1Head);
    }
    return dataSize;
}

int CB_RX1_GetRemainingSize(void)
{
//return size of remaining size in circular buffer
    int remainingSize;

    if (cbRx1Head >= cbRx1Tail) {
        remainingSize = CBRX1_BUFFER_SIZE - (cbRx1Head - cbRx1Tail) - 1;
    } else {
        remainingSize = cbRx1Tail - cbRx1Head - 1;
    }   
    return remainingSize;
}
