/*
  LeftTurn.h - This file help maintain and control the state and definition
  of how a Left turn looks on the light strip
  Created by Lisander Lopez, January 21, 2022.
  Released into the public domain.
*/

#ifndef LeftTurn_h
#define LeftTurn_h


class LeftTurn
{
private:
    // LedLibrary* _library;
    bool _isTurningLeft = false;
    bool _isTurningRight = false;
    int _turnStage = 0;
    Stream* _BTPORT;
    unsigned long _pastTime;
    unsigned long _currentTime;
    const unsigned long _period = 2000;
    // void sendDataToLights(const byte data[], int sizeOfData);
    void processLeftTurn();
public:
    LeftTurn();
    ~LeftTurn();
    void setLeftTurn(bool val);
    void tick();
    void begin();
};

#endif