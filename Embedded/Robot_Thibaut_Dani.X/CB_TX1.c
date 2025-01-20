#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_TX1.h"
#define CBTX1_BUFFER_SIZE 128
int cbTx1Head;
int cbTx1Tail;
unsigned char cbTx1Buffer[CBTX1_BUFFER_SIZE];
unsigned char isTransmitting = 0;

void CB_TX1_Init(void) {
    cbTx1Head = 0; // Initialiser l'indice d'insertion (�criture)
    cbTx1Tail = 0; // Initialiser l'indice de lecture
    isTransmitting = 0; // Initialiser l'indicateur de transmission
}

void SendMessage(unsigned char* message, int length) {
    unsigned char i = 0;
    if (CB_TX1_GetRemainingSize() > length) {
        //On peut �crire le message
        for (i = 0; i < length; i++)
            CB_TX1_Add(message[i]);
        if (!CB_TX1_IsTransmitting())
            SendOne();
    }
}

void CB_TX1_Add(unsigned char value) {
    int nextHead = (cbTx1Head + 1) % CBTX1_BUFFER_SIZE;

    if (nextHead != cbTx1Tail) { // V�rifier si le buffer n'est pas plein
        cbTx1Buffer[cbTx1Head] = value; // Ajouter la valeur au buffer
        cbTx1Head = nextHead; // Mettre � jour l'indice d'insertion
    }
}

unsigned char CB_TX1_Get(void) {
    unsigned char value = 0;

    if (cbTx1Tail != cbTx1Head) { // V�rifier si le buffer n'est pas vide
        value = cbTx1Buffer[cbTx1Tail]; // R�cup�rer la valeur
        cbTx1Tail = (cbTx1Tail + 1) % CBTX1_BUFFER_SIZE; // Mettre � jour l'indice de lecture
    }

    return value;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // clear TX interrupt flag
    if (cbTx1Tail != cbTx1Head) {
        SendOne();
    } else
        isTransmitting = 0;
}

void SendOne() {
    isTransmitting = 1;
    unsigned char value = CB_TX1_Get();
    U1TXREG = value; // Transmit one character
}

unsigned char CB_TX1_IsTransmitting(void) {
    return isTransmitting;
}

int CB_TX1_GetDataSize(void) {
    //return size of data stored in circular buffer
    int dataSize;
    if (cbTx1Head >= cbTx1Tail) {
        dataSize = cbTx1Head - cbTx1Tail;
    } else {
        dataSize = CBTX1_BUFFER_SIZE - (cbTx1Tail - cbTx1Head);
    }
    return dataSize;
}

int CB_TX1_GetRemainingSize(void) {
    //return size of remaining size in circular buffer
    int remainingSize;
    if (cbTx1Head >= cbTx1Tail) {
        remainingSize = CBTX1_BUFFER_SIZE - (cbTx1Head - cbTx1Tail) - 1;
    } else {
        remainingSize = cbTx1Tail - cbTx1Head - 1;
    }
    return remainingSize;
}

