#ifndef MIDI_T_H
    #define MIDI_T_H
    #include <stdint.h>
    enum class MidiType : uint8_t {
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
#endif