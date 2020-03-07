//#define byte uint8_t
//#define uint8_t byte
#include "SerialComm.h"

// Adapted from https://forum.arduino.cc/index.php?topic=225329.0
// More reliable communication over serial - sends message with start, length of data, actual data, and end
// Uses a start marker 254 and an end marker of 255
// Uses 253 as a special byte to be able to reproduce 253, 254 and 255

//================

//Board reset function definition
void(* resetFunc) (void) = 0;

#define startMarker 254
#define endMarker 255
#define specialByte 253
#define maxMessage 16

byte bytesRecvd = 0;
byte dataSentNum = 0; // the transmitted value of the number of bytes in the package i.e. the 2nd byte received
byte dataRecvCount = 0;

byte dataRecvd[maxMessage]; 
byte dataSend[maxMessage];  
byte tempBuffer[maxMessage];

byte dataSendCount = 0; // the number of 'real' bytes to be sent to the PC
byte dataTotalSend = 0; // the number of bytes to send to PC taking account of encoded bytes

boolean inProgress = false;
boolean startFound = false;
boolean allReceived = false;

void encodeHighBytes() {
  // Copies to temBuffer[] all of the data in dataSend[]
  //  and converts any bytes of 253 or more into a pair of bytes, 253 0, 253 1 or 253 2 as appropriate
  dataTotalSend = 0;
  for (byte n = 0; n < dataSendCount; n++) {
    if (dataSend[n] >= specialByte) {
      tempBuffer[dataTotalSend] = specialByte;
      dataTotalSend++;
      tempBuffer[dataTotalSend] = dataSend[n] - specialByte;
    }
    else {
      tempBuffer[dataTotalSend] = dataSend[n];
    }
    dataTotalSend++;
  }
}

void decodeHighBytes() {
  // Copies to dataRecvd[] only the data bytes i.e. excluding the marker bytes and the count byte
  //  and converts any bytes of 253 etc into the intended numbers
  //  Note that bytesRecvd is the total of all the bytes including the markers
  dataRecvCount = 0;
  for (byte n = 2; n < bytesRecvd - 1 ; n++) { // 2 skips the start marker and the count byte, -1 omits the end marker
    byte x = tempBuffer[n];
    if (x == specialByte) {
       // debugToPC("FoundSpecialByte");
       n++;
       x = x + tempBuffer[n];
    }
    dataRecvd[dataRecvCount] = x;
    dataRecvCount ++;
  }
}

void dataToPC() {
    // Sends data in dataSend[]
    //  uses encodeHighBytes() to copy data to tempBuffer
    //  sends data to PC from tempBuffer
    encodeHighBytes();

    Serial.write(startMarker);
    Serial.write(dataSendCount);
    Serial.write(tempBuffer, dataTotalSend);
    Serial.write(endMarker);
}

void debugToPC( char arr[]) {
    byte nb = 0;
    Serial.write(startMarker);
    Serial.write(nb);
    Serial.print(arr);
    Serial.write(endMarker);
}

void debugToPC( uint8_t num) {
    byte nb = 0;
    Serial.write(startMarker);
    Serial.write(nb);
    Serial.print(num);
    Serial.write(endMarker);
}

void updateStatus() {
  // Sends current status to control program over serial
  // onoff,now_playing,delayMillis,hue,saturation,value,red,green,blue
  dataSendCount = 9;
  dataSend[0] = 0;
  dataSend[1] = onoff;
  dataSend[2] = now_playing;
  dataSend[3] = (int)(delayMillis / 10);
  dataSend[4] = delta;
  dataSend[5] = rate;
  dataSend[6] = hue;
  dataSend[7] = saturation;
  dataSend[8] = value;
  dataToPC();
}

void getSerialData() {
  // Receives data into tempBuffer[]
  //  saves the number of bytes that the PC said it sent - which will be in tempBuffer[1]
  //  uses decodeHighBytes() to copy data from tempBuffer to dataRecvd[]
  if(Serial.available() > 0) {
    byte x = Serial.read();
    if (x == startMarker) { 
      bytesRecvd = 0; 
      inProgress = true;
      // blinkLED(2);
      // debugToPC("start received");
    }
    if(inProgress) {
      tempBuffer[bytesRecvd] = x;
      bytesRecvd ++;
    }
    if (x == endMarker) {
      inProgress = false;
      allReceived = true;
      // store the number of bytes that were sent
      dataSentNum = tempBuffer[1];
      decodeHighBytes();
    }
  }
}

void processData() {
  // processes the data that is in dataRecvd[]
  if (allReceived) {
    dataSendCount = dataRecvCount;
    for (byte n = 0; n < dataRecvCount; n++) {
      //Actual message
      if (dataRecvd[n] == 0) { onoff = false; FastLED.clear(); FastLED.show(); }
      else if (dataRecvd[n] == 1) { onoff = true; FastLED.clear(); FastLED.show(); }
      else if (dataRecvd[n] == 2) { updateStatus(); }
      else if (dataRecvd[n] == 3) { resetFunc(); }
      else if (dataRecvd[n] == 4) {
        //Found 4 byte, set HSV with next three values
        n++;
        hue = dataRecvd[n];
        n++;
        saturation = dataRecvd[n];
        n++;
        value = dataRecvd[n];
      }
      else if (dataRecvd[n] == 5) {
        //Found 5 byte, set hue with next value
        n++;
        hue = dataRecvd[n];
      }
      else if (dataRecvd[n] == 6) {
        //Found 6 byte, set saturation with next value
        n++;
        saturation = dataRecvd[n];
      }
      else if (dataRecvd[n] == 7) {
        //Found 9 byte, set value with next value
        n++;
        value = dataRecvd[n];
      }
      else if (dataRecvd[n] == 8) { 
        //Found 8 byte, set delayMillis with next value * 10 (0 - 2550)
        n++;
        delayMillis = dataRecvd[n] * 10;
      }
      else if (dataRecvd[n] == 9) {
        //Found 9 byte, set delta with next value
        n++;
        delta = dataRecvd[n];
      }
      else if (dataRecvd[n] == 10) {
        //Found 9 byte, set delta with next value
        n++;
        rate = dataRecvd[n];
      }
      else if (dataRecvd[n] == 50) { now_playing = 0; } //Solid
      else if (dataRecvd[n] == 51) { now_playing = 1; } //Rainbow
      else if (dataRecvd[n] == 52) { now_playing = 2; } //Pulse
      else if (dataRecvd[n] == 53) { now_playing = 3; } //Twinkle
      else if (dataRecvd[n] == 54) { now_playing = 4; } //Plasma
      else if (dataRecvd[n] == 55) { now_playing = 5; } //Dot
      else if (dataRecvd[n] == 56) { now_playing = 6; } //Fire
      //dataSend[n] = dataRecvd[n];
      // ^^^ for demonstration just copy dataRecvd to dataSend
    }
    //dataToPC();
    //byte somenum = 201;
    //debugToPC(somenum);
    //delay(100);
    allReceived = false; 
  }
}

