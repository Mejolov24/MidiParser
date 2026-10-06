#ifndef MIDI_PARSER_H
    #define MIDI_PARSER_H

#include <stdint.h>
#include <MidiType.h>

class MidiParser {
    public:

    typedef void (*MidiCallback)(MidiMessage msg);

    MidiParser() : _callback(nullptr), _runningStatus(0), _currentStatus(0), _state(WAIT_STATUS), _data1(0) {}

    void setCallback(MidiCallback cb) { _callback = cb; }

    void process(uint8_t byte) {
        if (byte >= 0xF8){dispatch(byte,0,0); return;} // realtime
        uint8_t type = byte & 0xF0;

        if (_state == WAIT_SYSEX){
            if(byte == 0xF7){
                dispatch(0xF7,0,0); // SysexEnd
                _state = WAIT_STATUS;
            }
            return;
        }

        if (byte >= 0x80) { // status byte
            if(byte < 0xF0){// data messages
                _runningStatus = byte;
                _currentStatus = byte;
                if ((byte & 0xF0) == 0xC0 || (byte & 0xF0) == 0xD0) { _state = WAIT_DATA1_SINGLE;}
                else {_state = WAIT_DATA1;}
            }
            else{
                bool dispatching = false;
                switch (byte){//system common mesagges
                    
                    case 0xF0: // System Exclusive Start
                        _runningStatus = 0;
                        _currentStatus = byte;
                        _state = WAIT_SYSEX;
                        dispatching = true;
                        break;
                    case 0xF1: // Time Code Quarter Frame (1 data byte)
                    case 0xF3: // Song Select (1 data byte)
                        _currentStatus = byte;
                        _state = WAIT_DATA1_SINGLE;
                        break;
                    case 0xF2: // Song Position Pointer (2 data bytes)
                        _currentStatus = byte;
                        _state = WAIT_DATA1;
                        break;
                    case 0xF6: // Tune Request (0 data bytes)
                        _state = WAIT_STATUS;
                        dispatching = true;
                        break;
                    case 0xF7: // End of System Exclusive (just in case it's out of band)
                        _runningStatus = 0;
                        _currentStatus = 0;
                        _state = WAIT_STATUS;
                        dispatching = true;
                        break;
                    default:
                        _state = WAIT_STATUS;
                        break;
                }
                if(dispatching){
                    dispatch(_currentStatus,0,0);
                    if(_currentStatus >= 0xF0){_currentStatus = _runningStatus;}
                }
            }
            return;
        }
        bool dispatching = false;
        switch (_state){
        case WAIT_DATA1:
            _data1 = byte;
            _state = WAIT_DATA2;
            break;
        case WAIT_DATA1_SINGLE:
            if(_currentStatus >= 0xF0){_state = WAIT_STATUS;}
            else {_state = WAIT_DATA1_SINGLE;}
            dispatching = true;
            _data1 = byte;
            byte = 0;
            break;
        case WAIT_DATA2:
            _state = WAIT_DATA1;
            dispatching = true;
            break;
        
        default:
            break;
        }
        if(dispatching){
            dispatch(_currentStatus,_data1,byte);
            if(_currentStatus >= 0xF0){_currentStatus = _runningStatus;}
        }
    }

private:
    MidiCallback _callback;
    uint8_t _runningStatus;
    uint8_t _currentStatus;
    uint8_t _data1;
    enum State {
        WAIT_STATUS,
        WAIT_DATA1,
        WAIT_DATA1_SINGLE,
        WAIT_DATA2,
        WAIT_SYSEX
    } _state;

    void dispatch(uint8_t status, uint8_t d1, uint8_t d2) { // humanize the raw bytes.
        if(_callback){
            _callback({
                static_cast<uint8_t>(status >= 0xF0) ? status : (status & 0xF0), // keep full byte if system message
                static_cast<uint8_t>(status >= 0xF0) ? 0 : (status & 0x0F), // system messages have no channel
                d1,
                d2
            });
        }
    }
};

#endif