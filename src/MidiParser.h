#ifndef MIDI_PARSER_H
    #define MIDI_PARSER_H

#include <stdint.h>

class MidiParser {
    public:

    struct MidiMessage {
        uint8_t type;
        uint8_t channel;
        uint8_t data1;
        uint8_t data2;

        // 14-bit pitch bend
        int16_t getPitchBend() const {
            if (type == 0xE0) {
                return (int16_t)((data2 << 7) | data1) - 8192; 
            }
            return 0;
        }
    };
    typedef void (*MidiCallback)(MidiMessage msg);

    MidiParser() : _callback(nullptr), _runningStatus(0), _state(WAIT_STATUS), _data1(0) {}

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
                //TODO: implement current status for keeping special messages
                // check for program change or channel pressure (1 byte) or multi byte (2 bytes)
                if (type == 0xC0 ||type == 0xD0) {_state = WAIT_DATA1_SINGLE;} else {_state = WAIT_DATA1;}
            }
            else{
                bool dispatching = false;
                if (byte == 0xF0 || byte == 0xF7) {_runningStatus = 0x00;}
                switch (byte){//system common mesagges
                    
                    case 0xF0: // System Exclusive Start
                        
                        _state = WAIT_SYSEX;
                        dispatching = true;
                        break;
                    case 0xF1: // Time Code Quarter Frame (1 data byte)
                    case 0xF3: // Song Select (1 data byte)
                        _state = WAIT_DATA1_SINGLE;
                        break;
                    case 0xF2: // Song Position Pointer (2 data bytes)
                        _state = WAIT_DATA1;
                        break;
                    case 0xF6: // Tune Request (0 data bytes)
                        _state = WAIT_STATUS;
                        dispatching = true;
                        break;
                    case 0xF7: // End of System Exclusive (just in case it's out of band)
                        _state = WAIT_STATUS;
                        dispatching = true;
                        break;
                    default:
                        _state = WAIT_STATUS;
                        break;
                }
                if(dispatching){dispatch(byte,0,0);}
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
            _state = WAIT_DATA1_SINGLE; // running status
            dispatching = true;
            _data1 = byte;
            byte = 0;
            break;
        case WAIT_DATA2:
            _state = WAIT_DATA1;
            dispatching = true;
        
        default:
            break;
        }
        if(dispatching){dispatch(_runningStatus,_data1,byte);}
    }

private:
    MidiCallback _callback;
    uint8_t _runningStatus;
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
                (status >= 0xF0) ? status : (status & 0xF0), // keep full byte if system message
                (status >= 0xF0) ? 0 : (status & 0x0F), // system messages have no channel
                d1,
                d2
            });
        }
    }
};

#endif