#pragma once

#include <stdint.h> // Variable Types
#include "Clock.h"
#include "Buzzer.h"
class Chip8
{
    private:

        uint8_t memory[4096]; // RAM

        uint8_t V[16]; // V Registers
        uint16_t I; // Index Register
        uint8_t sTimer; // Sound Timer
        uint8_t dTimer; // Delay Timer

        uint16_t PC; // Program Counter

        uint8_t SP; // Stack Pointer
        uint16_t stack[16]; // Stack

        uint16_t opcode; // Current opcode
    
        bool jumpFlag, skipFlag; // jumpFlag: Program Counter changed // skipFlag: Need to skip next inst.

        void reset(); // Resets Emu
    
	    void step(); // Processes one inst.
	
	    void timerStep(); // Decraments timers by one
	
        Clock clock; // Adjusts emulation speed

        Buzzer buzzer; // Buzzer WIP
    public:
        short display[64*32]; // Graphics Buffer

        bool drawFlag; // Indicates something is changed on screen

        uint8_t keypad[16]; // Keypad

        Chip8(const float _mhz);

        ~Chip8();
    
        void emulate();

        bool load(const char* filePath); // Loads rom to Memory
};

