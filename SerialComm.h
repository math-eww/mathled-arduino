#ifndef SERIALCOMM_H
#define SERIALCOMM_H
#include <FastLED.h>
#include "Globals.h"

void encodeHighBytes();

void decodeHighBytes();

void dataToPC();

void debugToPC( char arr[]);

void debugToPC( byte num);

void updateStatus();

void getSerialData();

void processData();

#endif
