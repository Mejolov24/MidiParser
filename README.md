# MidiParser
Simple C++ Midi GM 1.0 parser via callback

## Usage
First, declare your callback and assing it via setCallback()
Then, send individual midi bytes to process()
When data is parsed, the callback will be called.

## Midi message
```cpp
enum class MidiType : uint8_t {
    NoteOff           = 0x80,
    NoteOn            = 0x90,
    Aftertouch        = 0xA0,
    ControlChange     = 0xB0,
    ProgramChange     = 0xC0,
    ChannelPressure   = 0xD0,
    PitchBend         = 0xE0,
    SystemMessage     = 0xF0,
    Unknown           = 0x00
};
```

## Midi Type
```cpp
struct MidiMessage {
    MidiType type;
    uint8_t channel;
    uint8_t data1;
    uint8_t data2;
    int16_t getPitchBend() // Helper to get 14-bit pitch bend,shift to the left and fill gap, getting 14-bits
};
```

## Usage Example
```cpp
#include <MP.h>
#include <iostream>
MidiParser mp;

void ProcessMidi(MidiMessage msg) {
    switch (msg.type) {  
        case MidiType::NoteOn:
            std::cout << "Note ON recieved :" << "Channel : " << (int)msg.channel << "Note" << (int)msg.data1 << "Velocity" << (int)msg.data2 << std::endl;
            break;
        case MidiType::NoteOff:
            std::cout << "Note OFF recieved :" << "Channel : " << (int)msg.channel << "Note" << (int)msg.data1 << "Velocity" << (int)msg.data2 << std::endl;
            break;
        case MidiType::ProgramChange:
            std::cout << "ProgramChange recieved :" << "Channel : " << (int)msg.channel << "Data : " << (int)msg.data1 << std::endl;
            break;

        case MidiType::ControlChange:
            std::cout << "ControlChange recieved :" << "Channel : " << (int)msg.channel << "Data 1 : " << (int)msg.data1 << "Data 2 : " << (int)msg.data2 << std::endl;
            break;

        case MidiType::PitchBend:
            std::cout << "Pitchbend recieved :" << "Channel : " << (int)msg.channel << "Bend : " << (int)msg.getPitchBend() << std::endl;
            break;

        default:
            break;
    }
}

int main() {
    mp.setCallback(ProcessMidi);

    // 1. Trigger Note On (3 bytes)
    mp.process(0x90); mp.process(60); mp.process(100);

    // 2. Trigger Note Off (3 bytes)
    mp.process(0x80); mp.process(60); mp.process(0);

    // 3. Trigger Program Change (2 bytes)
    mp.process(0xC0); mp.process(5);

    // 4. Trigger Control Change (3 bytes)
    mp.process(0xB0); mp.process(7); mp.process(120);

    // 5. Trigger Pitch Bend (3 bytes)
    // Sending 0x00, 0x40 results in the center value (8192)
    mp.process(0xE0); mp.process(0x00); mp.process(0x40);

    return 0;
}
```