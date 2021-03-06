#ifndef BlinkerSerial_H
#define BlinkerSerial_H

#include <SoftwareSerial.h>
#include "HardwareSerial.h"
#include <Blinker/BlinkerProtocol.h>

SoftwareSerial *SSerialBLE;
// HardwareSerial *HSerialBLE;

class BlinkerTransportStream
{
    public :
        BlinkerTransportStream()
            : stream(NULL), isConnect(false)
        {}

        bool available()
        {
            if (!isHWS) {
                if (!SSerialBLE->isListening()) {
                    SSerialBLE->listen();
                    ::delay(100);
                }
            }
            
            if (stream->available()) {
                strcpy(streamData, (stream->readStringUntil('\n')).c_str());
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG2(BLINKER_F("handleSerial: "), streamData);
#endif
                return true;
            }
            else {
                return false;
            }
        }

        void begin(Stream& s, bool state)
        {
            stream = &s;
            stream->setTimeout(BLINKER_STREAM_TIMEOUT);
            isHWS = state;
        }

        String lastRead() { return STRING_format(streamData); }

        void print(String s)
        {
#ifdef BLINKER_DEBUG_ALL
            BLINKER_LOG2(BLINKER_F("Response: "), s);
#endif
            if(connected()) {
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG1(BLINKER_F("Succese..."));
#endif
                stream->print(s);
            }
            else {
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG1(BLINKER_F("Faile... Disconnected"));
#endif
            }
        }

        bool connect()
        {
            isConnect = true;
            return connected();
        }

        bool connected() { return isConnect; }

        void disconnect() { isConnect = false; }

    protected :
        Stream* stream;
        char    streamData[BLINKER_MAX_READ_SIZE];
        bool    isConnect;
        bool    isHWS = false;
};

class BlinkerSerail
    : public BlinkerProtocol<BlinkerTransportStream>
{
    typedef BlinkerProtocol<BlinkerTransportStream> Base;

    public :
        BlinkerSerail(BlinkerTransportStream& transp)
            : Base(transp)
        {}

        void begin(uint8_t ss_rx_pin = 2,
                    uint8_t ss_tx_pin = 3,
                    uint32_t ss_baud = 9600)
        {
#if defined (__AVR__)
            if (ss_rx_pin == 0 && ss_tx_pin == 1){
                Base::begin();
    #if defined (__AVR_ATmega32U4__)
                Serial1.begin(ss_baud);
                this->conn.begin(Serial1, true);
    #else
                Serial.begin(ss_baud);
                this->conn.begin(Serial, true);
    #endif
                BLINKER_LOG1("SerialBLE Initialled...");
                return;
            }
    #if defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__SAM3X8E__)
            else if (ss_rx_pin == 19 && ss_tx_pin == 18){
                Base::begin();
                Serial1.begin(ss_baud);
                this->conn.begin(Serial1, true);
                BLINKER_LOG1("SerialBLE Initialled...");
                return;
            }
            else if (ss_rx_pin == 17 && ss_tx_pin == 16){
                Base::begin();
                Serial2.begin(ss_baud);
                this->conn.begin(Serial2, true);
                BLINKER_LOG1("SerialBLE Initialled...");
                return;
            }
            else if (ss_rx_pin == 15 && ss_tx_pin == 14){
                Base::begin();
                Serial3.begin(ss_baud);
                this->conn.begin(Serial3, true);
                BLINKER_LOG1("SerialBLE Initialled...");
                return;
            }
    #endif  
            else {
                Base::begin();
                SSerialBLE = new SoftwareSerial(ss_rx_pin, ss_tx_pin);
                SSerialBLE->begin(ss_baud);
                this->conn.begin(*SSerialBLE, false);
                BLINKER_LOG1("SerialBLE Initialled...");
            }
#else
            Base::begin();
            SSerialBLE = new SoftwareSerial(ss_rx_pin, ss_tx_pin);
            SSerialBLE->begin(ss_baud);
            this->conn.begin(*SSerialBLE, false);
            BLINKER_LOG1("SerialBLE Initialled...");
#endif
        }
};

#endif