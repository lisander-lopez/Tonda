/*
  LedLibrary.h - This Library helps maintain state and funcitonality for
  controlling a Govee H7090 Car Light strip. Developed to be used with 
  teensy 4.0
  Created by Lisander Lopez, December 11, 2021.
  Released into the public domain.
*/

#ifndef LedLibrary_h
#define LedLibrary_h

#include <Arduino.h>
// #include "Effects/LeftTurn.h"

class LedLibrary
{
private:
    bool _isTurningLeft = false;
    bool _isTurningRight = false;
    int _turnStage = 0;
    bool _shouldResetLights;
    Stream* _BTPORT;
    unsigned long _pastTime;
    unsigned long _currentTime;
    const unsigned long _period = 2000;
    void sendDataToLights(const byte data[], int sizeOfData);
    void processLeftTurn();
    // LeftTurn* _leftTurnLibrary; 
public:
    LedLibrary(HardwareSerial* BT);
    ~LedLibrary();
    bool sendKeepAlive();
    void setLeftTurn(bool val);
    void setRightTurn(bool val);
    void ClearLEDs();
    void AllRed();
    void tick();
    void begin();
};

#endif