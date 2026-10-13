# MidiParser
Simple C++ Midi GM 1.0 parser via callback, provides a helper file "MidiType.h" with enums for the Midi Data and ther respective value.

## Usage
First, declare your callback and assing it via setCallback()
Then, send individual midi bytes to process()
When data finishes parsing, the callback will be called.

## Midi message
```cpp

enum MidiType : uint8_t {
    NoteOff              = 0x80,
    NoteOn               = 0x90,
    Aftertouch           = 0xA0,
    ControlChange        = 0xB0,
    ProgramChange        = 0xC0,
    ChannelPressure      = 0xD0,
    PitchBend            = 0xE0,
    SystemExclusive      = 0xF0,
    TimeCodeQuarterFrame = 0xF1,
    SongPosition         = 0xF2,
    SongSelect           = 0xF3,
    TuneRequest          = 0xF6,
    EndOfSystemExclusive = 0xF7,
    TimingClock          = 0xF8,
    Start                = 0xFA,
    Continue             = 0xFB,
    Stop                 = 0xFC,
    ActiveSensing        = 0xFE,
    SystemReset          = 0xFF,
    Unknown              = 0x00
};
```

## Control Changes
There are many control changes, please check the "MidiType.h" header file.

## Midi Type
```cpp
struct MidiMessage {
    uint8_t type;
    uint8_t channel;
    uint8_t data1;
    uint8_t data2;
    int16_t getPitchBend() // Helper to get 14-bit pitch bend
};
```

## Usage Example
```cpp
#include <MP.h>
#include <iostream>
MidiParser mp;

void ProcessMidi(MidiMessage msg) {
    MidiType message = static_cast<MidiType>(msg.type);
    MidiControlChange control = static_cast<MidiControlChange>(msg.data1);
    switch (message) {  
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

## License
This project is licensed under the MIT License.