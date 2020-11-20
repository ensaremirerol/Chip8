#include <iostream>
#include <random>


#include "Chip8.h"
#include "Clock.h"
#include "Buzzer.h"

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};



Chip8::Chip8(const float _mhz){
    clock = Clock();
    clock.init(_mhz);
    buzzer = Buzzer();
	}
Chip8::~Chip8(){}

void Chip8::reset(){
    PC = 0x200;
    opcode = 0;
    I = 0;
    SP = 0;
    
    for (int i = 0; i < 64*32; i++) // Clear Display
    {
        display[i] = 0;
    }

    for (int i = 0; i < 4096; i++)  // Clear Memory
    {
        memory[i] = 0;
    }

    for (int i = 0; i < 16; i++)    // Clear Stack, V and keypad
    {
        stack[i] = 0;
        V[i] = 0;
        keypad[i] = 0;
    }

    for (int i = 0; i < 80; i++)    // Load fonset to Memory
    {
        memory[i] = chip8_fontset[i];
    }
    
    // Reset Timer
    dTimer = 0;
    sTimer = 0;
    
    srand (std::chrono::steady_clock::now().time_since_epoch().count());

}

bool Chip8::load(const char* filePath){

    reset();

    printf("Loading\n");

    // Open ROM

    FILE* rom;

    fopen_s(&rom, filePath, "rb");

    if(rom == NULL){
        std::cerr << "Failed to open ROM file" << std::endl;
        return false;
    }

    // Get File Size

    fseek(rom, 0, SEEK_END);
    long romSize = ftell(rom);
    rewind(rom);

    // Allocate Memory for rom

    char* romBuffer = (char*) malloc(sizeof(char) * romSize);
    if (romBuffer == NULL){
        std::cerr << "Failed to allocate memory for ROM file" << std::endl;
        return false;
    }

    // Read ROM
    size_t result = fread(romBuffer, sizeof(char), (size_t)romSize, rom);
    if (result != romSize){
        std::cerr << "Failed to read ROM" << std::endl;
        return false;
    }

    // Copy buffer to memory

    if((4096-512) > romSize){
        for (int i = 0; i < romSize; i++)
        {
            memory[512+i] = romBuffer[i];
        }
        
    }
    else{
        std::cerr << "ROM file is too large to fit in memory" << std::endl;
        return false;
    }

    // Clean up

    fclose(rom);
    free(romBuffer);
    return true;
}

void Chip8::step(){
	
	jumpFlag = false; // It is true when Program Counter (PC) jumped
	skipFlag = false; // It is true when condition of Skip Instraction (PC += 4)
	
    opcode = memory[PC] << 8 | memory[PC+1]; // OpCode s length is 16bits

    unsigned short X = ((opcode & 0x0F00) >> 8); // Most used bits are dedected here
    unsigned short Y = ((opcode & 0x00F0) >> 4);

    
    switch (opcode & 0XF000)
    {
    case 0x0000:

        // 00E
        switch (opcode & 0x000F)
        {

        case 0X0000: // Clear Screen
            for (int i = 0; i < 64*32; i++)
            {
                display[i] = 0;
            }
            drawFlag = true;
            break;
        case 0X000E:
            PC = stack[--SP];
            break;
        default:
            printf("\nUnknown opcode %.4X\n", opcode);
            exit(3);
        }
        break;

    // 1NNN Jump to address NNN
    case 0x1000:
        PC = opcode & 0x0FFF;
        jumpFlag = true;
        break;
    
    // 2NNN Call subroutine at NNN
    case 0x2000:
        stack[SP++] = PC;
        PC = opcode & 0x0FFF;
        jumpFlag = true;
        break;

    // 3XKK If V[X] equals KK skips next inst.
    case 0x3000:
        if (V[X] == (opcode & 0X00FF)){
            skipFlag = true;
        }
        break;

    // 4XKK If V[X] not equals KK skips next inst.
    case 0x4000:
        if (V[X] != (opcode & 0X00FF)){
            skipFlag = true;
        }
        break;

    // 5XY0 If V[X] equals V[Y] skips next inst. 
    case 0x5000:
        if (V[X] == V[Y]){
            skipFlag = true;
        }
    
    // 6XKK V[X] = KK
    case 0x6000:
        V[X] = (opcode & 0X00FF);
        break;
    
    // 7XKK V[X] = V[X] + KK 
    case 0x7000:
        V[X] += (opcode & 0X00FF);
        break;

    // 800
    case 0x8000:
        switch (opcode & 0x000F)
        {
        
        // 8XY0 V[X] = V[Y];
        case 0x0000:
            V[X] = V[Y];
            break;
        
        // 8XY1 V[X] = V[X] | V[Y];
        case 0X0001:
            V[X] = V[X] | V[Y];
            break;

        // 8XY2 V[X] = V[X] & V[Y];
        case 0x0002:
            V[X] = V[X] & V[Y];
            break;

        // 8XY3 V[X] = V[X] ^ V[Y];
        case 0x003:
            V[X] = V[X] ^ V[Y];
            break;

        // 8XY4 V[X] = V[X] + V[Y] If result is greater than 8 bits(255) V[F] is set to 1 otherwise 0
        case 0x0004:
			V[X] += V[Y];          
            if (V[Y] >  (0xFF - V[X])){
                V[15] = 1;
            }
            else
            {
                V[15] = 0;
            }
            
            break;


        // 8XY5 V[X] = V[X] - V[Y] If V[X] greater than V[Y], V[F] is set to 1 otherwise 0
        case 0x0005:
            if (V[Y] > V[X]){
                V[15] = 0;
            }
            else
            {
                V[15] = 1;
            }
            V[X] -=  V[Y];
            break;
        
        // 8XY6 V[X] >> 1 If V[X] is odd V[F] is set to 1 otherwise 0
        case 0x0006:
            V[15] = V[X] & 1;
            V[X] = V[X] >> 1;
            break;

        // 8XY5 V[X] = V[Y] - V[X] If V[Y] greater than V[X], V[F] is set to 1 otherwise 0
        case 0x0007:
            if (V[X] > V[Y]){
                V[15] = 0;
            }
            else
            {
                V[15] = 1;
            }
            V[Y] -=  V[X];
            break;
        
        // 8XYE V[X] << 1 If most significant bit of V[X] is 1 V[F] is set to 1 otherwise 0
        case 0x000E:
            V[15] = V[X] >> 7;
            V[X] = V[X] << 1;
            break;
        default:
            printf("\nUnknown opcode %.4X\n", opcode);
            exit(3);
        }
        break;
    
    // 9XY0 If V[X] not equals V[Y] skips next inst.
    case 0x9000:
        if (V[X] != V[Y]){
            skipFlag = true;
        }
        break;
    
    // ANNN I = NNN
    case 0xA000:
        I = opcode & 0x0FFF;
        break;

    // BNNN PC = NNN + V[0]
    case 0xB000:
        PC = (opcode & 0x0FFF) + V[0];
        jumpFlag = true;
        break;
    
    // CXKK V[X] = Random & KK (0 <= Random <= 255);
    case 0xC000:
        V[X] = (rand() % (0xFF + 1)) & (opcode & 0X00FF);
        break;
    
    // DXYN Draw a sprite from memory starting at pos V[X], V[Y] If there is collision V[F] = 1;
    case 0xD000:
    	{
    		drawFlag = true;
	    	unsigned short posX = V[X];
	        unsigned short posY = V[Y];
	        unsigned short height = opcode & 0x000F;
	        unsigned short pixelRow;
	        V[15] = 0;
	        
	        for (int y = 0; y < height; y++)
	        {
	            pixelRow = memory[I + y];
	            for (int x = 0; x < 8; x++){
	                if(((pixelRow >> 7) & 1) != 0){
	                    if(display[posX + x + ((posY + y) * 64)] == 1){
	                        V[15] = 1;
	                    }
	                    display[posX + x + ((posY + y) * 64)] ^= 1;
	                }
	                pixelRow <<= 1;
	            }
	        }       
	        break;
		}
        
        
    // E000
    case 0xE000:
         
         switch (opcode & 0x00FF)
         {
             // EX9E Skip inst. if key with the value of V[X] is pressed
             case 0x009E:
                if(keypad[V[X]] == 1){
                    skipFlag = true;
                }
                break;
            
             // EX9E Skip inst. if key with the value of V[X] is not pressed
            case 0x00A1:
                if(keypad[V[X]] == 0){
                    skipFlag = true;
                }
                break;
            default:
                printf("\nUnknown opcode %.4X\n", opcode);
                exit(3);
         }
         break;

    // F000
    case 0xF000:
        switch (opcode & 0x00FF)
        {

        // V[X] = dTimer    
        case 0x0007:
            V[X] = dTimer;
            break;
        
        // Wait for a keypress and store the value of key in V[X]
        case 0x000A:
        	{
	        	bool keyPressed = false;
	            for (int i = 0; i < 16; i++)
	            {
	                if(keypad[i] == 1){
	                    keyPressed = true;
	                    V[X] = i;
	                }
	            }
	
	            if (!keyPressed){
	                return;
	            }
	            
	            break;	
			}
              
        // FX15 dTimer = V[X]
        case 0x0015:
            dTimer = V[X];
            break;
        
        // FX18 sTimer = V[X]
        case 0x0018:
            sTimer = V[X];
            break;
         // FX1E I = I + V[X]
        case 0x001E:
            I += V[X];
            break;
        
        // FX29 Set I to location of sprite for digit V[X]
        case 0x0029:
            I = V[X] * 0x5;
            break;
        
        // FX33 Store BCD representation of V[X] in memory locations I, I+1, and I+2.
        case 0x0033:
            memory[I] = V[X] / 100;
            memory[I + 1] = (V[X] / 10) % 10;
            memory[I + 2] = V[X] % 10;
            break;
        
        // FX55 Store registers V[0] through V[X] in memory location starting at I 
        case 0x0055:
            for(int i = 0; i <= X; i++){
                memory[I + i] = V[i];
            }
            break;
        
        // FX65 Read registers V[0] through V[X] from memory location starting at I
        case 0x0065:
            for(int i = 0; i <= X; i++){
                V[i] = memory[I + i];
            }
            break;

        default:
            printf("\nUnknown opcode %.4X\n", opcode);
       	 	exit(3);
        }
        break;
    default:
        printf("\nUnknown opcode %.4X\n", opcode);
        exit(3);
    }
    if(skipFlag){
    	PC += 4;
	}
	else if(!jumpFlag){
		PC += 2;
	}
        
}

void Chip8::timerStep(){
	if(dTimer > 0){
		dTimer--;	
	} 
	if(sTimer > 0){
        buzzer.beep();
		sTimer--;
	}
}

void Chip8::emulate(){
    if (clock.checkCpuClock()) {
        step();
    }
    if (clock.checkRegisterClock()) {
        timerStep();
    }
	
}
