#include <Arduino.h>
#include <LedLibrary.h>

const byte firstSegment = 0b000001;
const byte secondSegment = 0b000010;
const byte thirdSegment = 0b000100;
const byte fourthSegment = 0b001000;
const byte fifthSegment = 0b010000;
const byte sixthSegment = 0b100000;

const byte resetLEDPacket[] = {0x33,0x05,0x15,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x1D};
const byte redPacket[] = {0x33,0x05,0x15,0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0xE2};
const byte keepAlivePacket[] = {0xAA, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAB};

const byte leftTurnPacket[4][20] = {
    {0x33,0x05,0x15,0x01,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,firstSegment,0x00,0x00,0x00,0x00,0x00,0x00,0xDC},
    {0x33,0x05,0x15,0x01,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,secondSegment,0x00,0x00,0x00,0x00,0x00,0x00,0xDF},
    {0x33,0x05,0x15,0x01,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,thirdSegment,0x00,0x00,0x00,0x00,0x00,0x00,0xD9},
    {0x33,0x05,0x15,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x1D}
}; 

const int leftTurnFirstDelay = 300; // Time in between each "stage"
const int leftTurnSecondDelay = 100; // Time to wait after last "stage"

unsigned long _leftPastTime;
unsigned long _leftCurrentTime;

LedLibrary::LedLibrary(HardwareSerial* BT)
{
    _BTPORT = BT;
}

LedLibrary::~LedLibrary()
{
    
}

void LedLibrary::ClearLEDs()
{
    sendDataToLights(resetLEDPacket, sizeof(resetLEDPacket));
}

void LedLibrary::AllRed()
{
    sendDataToLights(redPacket, sizeof(redPacket));
}

void LedLibrary::sendDataToLights(const byte data[], int sizeOfData)
{
    String base = "AT+SEND_DATAWN000E";
    _BTPORT->print(base);
    _BTPORT->write(data, sizeOfData);
    _BTPORT->flush();
}

void LedLibrary::tick() 
{ 
    _currentTime = millis();
    if(_isTurningLeft){
        processLeftTurn();
    } else {
        if (_currentTime - _pastTime >= _period)
        {
            sendDataToLights(keepAlivePacket, sizeof(keepAlivePacket));
            _pastTime = _currentTime;  //IMPORTANT to save the start time of the current LED state.
         }
    }
}

void LedLibrary::begin() 
{
    _pastTime = millis();
    _leftPastTime = millis();
    ClearLEDs();
}

bool LedLibrary::sendKeepAlive()
{
    sendDataToLights(keepAlivePacket, sizeof(keepAlivePacket));
    return true;
}

void LedLibrary::setLeftTurn(bool val)
{
    if(val == _isTurningLeft){
        return;
    }
    
    if(val)
    {
        _turnStage = 0;
    } else {
        // Very hacky I don't like but using millis was a PAIN to ensure to reset after the latest send since CAN interrupts
        delay(10);
        ClearLEDs();
        delay(10);

    }
    _isTurningLeft = val;
}

void LedLibrary::processLeftTurn()
{
    _leftCurrentTime = millis();

    if(_turnStage == 3){
        // < instead of >= because do nothing if its not time to go to next stage
        if (_leftCurrentTime - _leftPastTime >= leftTurnFirstDelay)
        {
            sendDataToLights(leftTurnPacket[_turnStage], sizeof(leftTurnPacket[_turnStage]));
            _leftPastTime = _leftCurrentTime;
            _turnStage = (_turnStage + 1) % 4;
            return;
        }
    } else {
        if (_leftCurrentTime - _leftPastTime >= leftTurnSecondDelay)
        {
            sendDataToLights(leftTurnPacket[_turnStage], sizeof(leftTurnPacket[_turnStage]));
            _leftPastTime = _leftCurrentTime;
            _turnStage = (_turnStage + 1) % 4;
            return;
        }
    }
}