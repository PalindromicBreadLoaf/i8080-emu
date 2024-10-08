#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdint>

typedef struct ConditionCodes {
    uint8_t     z:1;    //(zero) set to 1 when the result is equal to zero
    uint8_t     s:1;    //(sign) set to 1 when bit 7 (the most significant bit or MSB) of the math instruction is set
    uint8_t     p:1;    //(parity) is set when the answer has even parity, clear when odd parity
    uint8_t     cy:1;   //(carry) set to 1 when the instruction resulted in a carry out or borrow into the high order bit
    uint8_t     ac:1;   //(auxillary carry) is used mostly for BCD (binary coded decimal) math
    uint8_t     pad:3;
} ConditionCodes;

typedef struct State8080 {
    uint8_t     a;
    uint8_t     b;
    uint8_t     c;
    uint8_t     d;
    uint8_t     e;
    uint8_t     h;
    uint8_t     l;
    uint16_t    sp;
    uint16_t    pc;
    uint8_t     *memory;
    struct      ConditionCodes  cc;
    uint8_t     int_enable;
} State8080;

void UnimplementedInstruction(State8080* state) {
    state->pc--;

    std::cout << "Unimplemented Instruction" << std::endl;
    exit(1);
}

bool parity(const int val) {
    uint8_t one_bits = 0;
    for (int i = 0; i < 8; i++) {
        one_bits += ((val >> i) & 1);
    }

    return (one_bits & 1) == 0;
}

int Emulate8080Op(State8080* state) {
    unsigned char *opcode = &state->memory[state->pc];
    uint16_t answer;
    uint16_t offset;
    uint16_t ret;
    uint16_t bc, de, hl, hi;
    uint8_t x;

    switch (*opcode) {
        case 0x00: break;   //NOP
        case 0x01:          //LXI   B,word
            state->c = opcode[1];
            state->b = opcode[2];
            state->pc+=2;
            break;
        case 0x02: UnimplementedInstruction(state); break;
        case 0x03:          //INX B
            bc = (static_cast<uint16_t>(state->b) << 8) | state->c;
            bc+=1;
            state->b = (bc >> 8) & 0xff;
            state->c = bc & 0xff;
            break;
        case 0x04:          //INR B
            answer = static_cast<uint16_t> (state->b) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->b = answer & 0xff;
            break;
        case 0x05:          //DCR B
            answer = static_cast<uint16_t> (state->b) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->b = answer & 0xff;
            break;
        case 0x06: UnimplementedInstruction(state); break;
        case 0x07: UnimplementedInstruction(state); break;
        case 0x08: UnimplementedInstruction(state); break;
        case 0x09:          //DAD B
            bc = (static_cast<uint16_t>(state->b) << 8) | state->c;
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl += bc;
            state->cc.cy = (hl>0xff);
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x0a: UnimplementedInstruction(state); break;
        case 0x0b:          //DCX B
            bc = (static_cast<uint16_t>(state->b) << 8) | state->c;
            bc-=1;
            state->b = (bc >> 8) & 0xff;
            state->c = bc & 0xff;
            break;
        case 0x0c:          //INR C
            answer = static_cast<uint16_t> (state->c) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->c = answer & 0xff;
            break;
        case 0x0d:          //DCR C
            answer = static_cast<uint16_t> (state->c) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->c = answer & 0xff;
            break;
        case 0x0e: UnimplementedInstruction(state); break;
        case 0x0f:          //RRC
            x = state->a;
            state->a = ((x & 1) << 7) | (x >> 1);
            state->cc.cy = (1 == (x & 1));
            break;
        case 0x10:
            break;
        case 0x11: UnimplementedInstruction(state); break;
        case 0x12: UnimplementedInstruction(state); break;
        case 0x13:          //INX D
            de = (static_cast<uint16_t>(state->d) << 8) | state->e;
            de+=1;
            state->d = (de >> 8) & 0xff;
            state->e = de & 0xff;
            break;
        case 0x14:          //INR D
            answer = static_cast<uint16_t> (state->d) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->d = answer & 0xff;
            break;
        case 0x15:          //DCR D
            answer = static_cast<uint16_t> (state->d) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->d = answer & 0xff;
            break;
        case 0x16: UnimplementedInstruction(state); break;
        case 0x17: UnimplementedInstruction(state); break;
        case 0x18: UnimplementedInstruction(state); break;
        case 0x19:          //DAD D
            de = (static_cast<uint16_t>(state->d) << 8) | state->e;
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl += de;
            state->cc.cy = (hl>0xff);
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x1a: UnimplementedInstruction(state); break;
        case 0x1b:          //DCX D
            de = (static_cast<uint16_t>(state->d) << 8) | state->e;
            de-=1;
            state->d = (de >> 8) & 0xff;
            state->e = de & 0xff;
            break;
        case 0x1c:          //INR E
            answer = static_cast<uint16_t> (state->e) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->e = answer & 0xff;
            break;
        case 0x1d:          //DCR E
            answer = static_cast<uint16_t> (state->e) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->e = answer & 0xff;
            break;
        case 0x1e: UnimplementedInstruction(state); break;
        case 0x1f:          //RAR
            x = state->a;
            state->a = (state->cc.cy << 7) | (x >> 1);
            state->cc.cy = (1 == (x & 1));
            break;
        case 0x20: UnimplementedInstruction(state); break;
        case 0x21: UnimplementedInstruction(state); break;
        case 0x22: UnimplementedInstruction(state); break;
        case 0x23:          //INX H
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl+=1;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x24:          //INR H
            answer = static_cast<uint16_t> (state->h) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->h = answer & 0xff;
            break;
        case 0x25:          //DCR H
            answer = static_cast<uint16_t> (state->h) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->h = answer & 0xff;
            break;
        case 0x26: UnimplementedInstruction(state); break;
        case 0x27: UnimplementedInstruction(state); break;
        case 0x28: UnimplementedInstruction(state); break;
        case 0x29:          //DAD H
            hi = (static_cast<uint16_t>(state->b) << 8) | state->c;
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl += hi;
            state->cc.cy = (hl>0xff);
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x2a: UnimplementedInstruction(state); break;
        case 0x2b:          //DCX H
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl-=1;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x2c:          //INR L
            answer = static_cast<uint16_t> (state->l) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->l = answer & 0xff;
            break;
        case 0x2d:          //DCR L
            answer = static_cast<uint16_t> (state->l) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->l = answer & 0xff;
            break;
        case 0x2e: UnimplementedInstruction(state); break;
        case 0x2f:          //CMA
            state->a = ~state->a;
            break;
        case 0x30:          //NO INSTRUCTION
            break;
        case 0x31: UnimplementedInstruction(state); break;
        case 0x32:          //INX SP
            state->sp+=1;
            break;
        case 0x33: UnimplementedInstruction(state); break;
        case 0x34:          //INR M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->memory[offset]) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->h = (answer >> 8) & 0xff;
            state->l = answer & 0xff;
            break;
        case 0x35:          //DCR M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->memory[offset]) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->h = (answer >> 8) & 0xff;
            state->l = answer & 0xff;
            break;
        case 0x36: UnimplementedInstruction(state); break;
        case 0x37:          //STC
            state->cc.cy = 1;
            break;
        case 0x38: UnimplementedInstruction(state); break;
        case 0x39:          //DAD SP
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl += state->sp;
            state->cc.cy = (hl>0xff);
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x3a: UnimplementedInstruction(state); break;
        case 0x3b:          //DCX SP
            state->sp-=1;
            break;
        case 0x3c:          //INR A
            answer = static_cast<uint16_t> (state->a) + 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x3d:          //DCR A
            answer = static_cast<uint16_t> (state->a) - 1;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x3e: UnimplementedInstruction(state); break;
        case 0x3f:          //CMC
            state->cc.cy = !state->cc.cy;
            break;
        case 0x40:          //MOV B,B
            state->b = state->b;
            break;
        case 0x41:          //MOV B,C
            state->b = state->c;
            break;
        case 0x42:          //MOV B,D
            state->b = state->d;
            break;
        case 0x43:          //MOV B,E
            state->b = state->e;
            break;
        case 0x44:          //MOV B,H
            state->b = state->h;
            break;
        case 0x45:          //MOV B,L
            state->b = state->l;
            break;
        case 0x46:          //MOV B,M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->b = answer * 0xff;
            break;
        case 0x47:          //MOV B,A
            state->b = state->a;
            break;
        case 0x48:          //MOV C,B
            state->c = state->b;
            break;
        case 0x49:          //MOV C,C
            state->c = state->c;
            break;
        case 0x4a:          //MOV C,D
            state->c = state->d;
            break;
        case 0x4b:          //MOV C,E
            state->c = state->e;
            break;
        case 0x4c:          //MOV C,H
            state->c = state->h;
            break;
        case 0x4d:          //MOV C,L
            state->c = state->l;
            break;
        case 0x4e:          //MOV C,M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->c = answer & 0xff;
            break;
        case 0x4f:          //MOV C,A
            state->c = state->a;
            break;
        case 0x50:          //MOV D,B
            state->d = state->b;
            break;
        case 0x51:          //MOV D,C
            state->d = state->c;
            break;
        case 0x52:          //MOV D,D
            state->d = state->d;
            break;
        case 0x53:          //MOV D,E
            state->d = state->e;
            break;
        case 0x54:          //MOV D,H
            state->d = state->h;
            break;
        case 0x55:          //MOV D,L
            state->d = state->l;
            break;
        case 0x56:          //MOV D,M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->d = answer;
            break;
        case 0x57:          //MOV D,A
            state->d = state->a;
            break;
        case 0x58:          //MOV E,B
            state->e = state->b;
            break;
        case 0x59:          //MOV E,C
            state->e = state->c;
            break;
        case 0x5a:          //MOV E,D
            state->e = state->d;
            break;
        case 0x5b:          //MOV E,E
            state->e = state->e;
            break;
        case 0x5c:          //MOV E,H
            state->e = state->h;
            break;
        case 0x5d:          //MOV E,L
            state->e = state->l;
            break;
        case 0x5e:
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->e = answer;
            break;
        case 0x5f:          //MOV E,A
            state->e = state->a;
            break;
        case 0x60:          //MOV H,B
            state->h = state->b;
            break;
        case 0x61:          //MOV H,C
            state->h = state->c;
            break;
        case 0x62:          //MOV H,D
            state->h = state->d;
            break;
        case 0x63:          //MOV H,E
            state->h = state->e;
            break;
        case 0x64:          //MOV H,H
            state->h = state->h;
            break;
        case 0x65:          //MOV H,L
            state->h = state->l;
            break;
        case 0x66:          //MOV H,M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->h = answer;
            break;
        case 0x67:          //MOV H,A
            state->h = state->a;
            break;
        case 0x68:          //MOV L,B
            state->l = state->b;
            break;
        case 0x69:          //MOV L,C
            state->l = state->c;
            break;
        case 0x6a:          //MOV L,D
            state->l = state->d;
            break;
        case 0x6b:          //MOV L,E
            state->l = state->e;
            break;
        case 0x6c:          //MOV L,H
            state->l = state->h;
            break;
        case 0x6d:          //MOV L,L
            state->l = state->l;
            break;
        case 0x6e:          //MOV L,M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->l = answer;
            break;
        case 0x6f:          //MOV L,A
            state->l = state->a;
            break;
        case 0x70:          //MOV M,B
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl = state->b;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x71:          //MOV M,C
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl = state->c;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x72:          //MOV M,D
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl = state->d;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x73:          //MOV M,E
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl = state->e;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x74:          //MOV M,H
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl = state->h;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x75:          //MOV M,L
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl = state->l;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x76:          //HLT
            exit(0);
        break;
        case 0x77:          //MOV M,A
            hl = (static_cast<uint16_t>(state->h) << 8) | state->l;
            hl = state->a;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
            break;
        case 0x78:          //MOV A,B
            state->a = state->b;
            break;
        case 0x79:          //MOV A,C
            state->a = state->c;
            break;
        case 0x7a:          //MOV A,D
            state->a = state->d;
            break;
        case 0x7b:          //MOV A,E
            state->a = state->e;
            break;
        case 0x7c:          //MOV A,H
            state->a = state->h;
            break;
        case 0x7d:          //MOV A,L
            state->a = state->l;
            break;
        case 0x7e:          //MOV A,M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->a = answer;
            break;
        case 0x7f:          //MOV A,A
            state->a = state->a;
            break;
        case 0x80:      //ADD B
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->b);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x81:      //ADD C
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->c);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x82:      //ADD D
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->d);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x83:      //ADD E
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->e);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x84:      //ADD H
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->h);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x85:      //ADD L
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->l);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x86:      //ADD M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x87:      //ADD A
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->a);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x88:      //ADC B
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->b) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x89:      //ADC C
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->c) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x8a:      //ADC D
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->d) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x8b:      //ADC E
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->e) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x8c:      //ADC H
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->h) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x8d:      //ADC L
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->l) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x8e:      //ADC M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->memory[offset]) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x8f:      //ADC A
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (state->a) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x90:      //SUB B
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->b);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x91:      //SUB C
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->c);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x92:       //SUB D
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->d);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x93:
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->e);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x94:      //SUB H
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->h);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x95:      //SUB L
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->l);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x96:      //SUB M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->memory[offset]);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x97:      //SUB A
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->a);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x98:      //SBB B
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->b) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x99:      //SBB C
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->c) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x9a:      //SBB D
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->d) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x9b:      //SBB E
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->e) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x9c:      //SBB H
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->h) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x9d:      //SBB L
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->l) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x9e:      //SBB M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->memory[offset]) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0x9f:      //SBB A
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (state->a) - state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa0:      //ANA B
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->b);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa1:          //ANA C
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->c);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa2:          //ANA D
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->d);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa3:          //ANA E
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->e);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa4:          //ANA H
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->h);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa5:          //ANA L
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->l);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa6:          //ANA M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->memory[offset]);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa7:          //ANA A
            answer = static_cast<uint16_t> (state->a) & static_cast<uint16_t> (state->a);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa8:          //XRA B
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->b);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xa9:          //XRA C
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->c);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xaa:          //XRA D
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->d);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xab:          //XRA E
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->e);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xac:          //XRA H
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->h);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xad:          //XRA L
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->l);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xae:          //XRA M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->memory[offset]);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xaf:          //XRA A
            answer = static_cast<uint16_t> (state->a) ^ static_cast<uint16_t> (state->a);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb0:          //ORA B
            answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->b);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb1:          //ORA C
            answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->c);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb2:          //ORA D
            answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->d);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb3:          //ORA E
            answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->e);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb4:          //ORA H
            answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->h);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb5:          //ORA L
            answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->l);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb6:          //ORA M
            offset = (state->h << 8) | (state->l);
            answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->memory[offset]);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb7:          //ORA A
                answer = static_cast<uint16_t> (state->a) | static_cast<uint16_t> (state->a);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xb8: UnimplementedInstruction(state); break;
        case 0xb9: UnimplementedInstruction(state); break;
        case 0xba: UnimplementedInstruction(state); break;
        case 0xbb: UnimplementedInstruction(state); break;
        case 0xbc: UnimplementedInstruction(state); break;
        case 0xbd: UnimplementedInstruction(state); break;
        case 0xbe: UnimplementedInstruction(state); break;
        case 0xbf: UnimplementedInstruction(state); break;
        case 0xc0:          //RNZ
            if (state->cc.z == 0)
                state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);

            state->pc+=2;
            break;
        case 0xc1: UnimplementedInstruction(state); break;
        case 0xc2:          //JNZ
            if (state->cc.z == 0)
                state->pc = (opcode[2] << 8 | opcode[1]);
            else
                state->pc+=2;
            break;
        case 0xc3:          //JMP
            state->pc = (opcode[2] << 8 | opcode[1]);
            break;
        case 0xc4:          //CNZ address
            if (state->cc.z == 0) {
                ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc+=2;
            break;
        case 0xc5: UnimplementedInstruction(state); break;
        case 0xc6:      //ADI
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (opcode[1]);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xc7:          //RST 0
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 0;
            break;
        case 0xc8:          //RZ
            if (state->cc.z == 1) {
                state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
                state->pc+=2;
            }
            else
                state->pc+=2;
            break;
        case 0xc9:          //RET
            state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
            state->pc+=2;
            break;
        case 0xca:          //JZ address
            if (state->cc.z == 1)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xcb: UnimplementedInstruction(state); break;
        case 0xcc:          //CZ address
            if (state->cc.z == 1) {
                ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc+=2;
            break;
        case 0xcd:          //CALL address
            ret = state->pc+2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp = state->sp - 2;
            state->pc = (opcode[2] << 8) | opcode[1];
            break;
        case 0xce:          //ACI
            answer = static_cast<uint16_t> (state->a) + static_cast<uint16_t> (opcode[1]) + state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xcf:          //RST 1
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 8;
            break;
        case 0xd0:          //RNC address
            if (state->cc.cy == 0) {
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc+=2;
            }
            else
                state->pc+=2;
            break;
        case 0xd1: UnimplementedInstruction(state); break;
        case 0xd2:          //JNC address
            if (state->cc.cy == 0)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xd3: UnimplementedInstruction(state); break;
        case 0xd4:          //CNC address
            if (state->cc.cy == 0)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xd5: UnimplementedInstruction(state); break;
        case 0xd6:          //SUI
            answer = static_cast<uint16_t> (state->a) - static_cast<uint16_t> (opcode[1]);
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff);
            state->a = answer & 0xff;
            break;
        case 0xd7:          //RST 2
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 16;
            break;
        case 0xd8:          //RC
            if (state->cc.cy == 1) {
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc+=2;
            }
            else
                state->pc+=2;
            break;
        case 0xd9: UnimplementedInstruction(state); break;
        case 0xda:          //JC address
            if (state->cc.cy == 1)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xdb: UnimplementedInstruction(state); break;
        case 0xdc:          //CC address
            if (state->cc.cy == 1) {
                ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc+=2;
            break;
        case 0xdd: UnimplementedInstruction(state); break;
        case 0xde: UnimplementedInstruction(state); break;
        case 0xdf:          //RST 3
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 24;
            break;
        case 0xe0:          //RPO
            if (state->cc.p == 0) {
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc+=2;
            }
            else
                state->pc+=2;
            break;
        case 0xe1: UnimplementedInstruction(state); break;
        case 0xe2:          //JPO address
            if (state->cc.p == 0)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xe3: UnimplementedInstruction(state); break;
        case 0xe4:          //CPO address
            if (state->cc.p == 0) {
                ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc+=2;
            break;
        case 0xe5: UnimplementedInstruction(state); break;
        case 0xe6:          //ANI byte
            x = state->a & opcode[1];
            state->cc.z = (x == 0);
            state->cc.s = (0x80 == (x & 0x80));
            state->cc.p = parity(x);
            state->cc.cy = 0;
            state->a = x;
            state->pc++;
            break;
        case 0xe7:          //RST 4
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 32;
            break;
        case 0xe8:          //RPE
            if (state->cc.p == 1)
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);

            state->pc+=2;
            break;
        case 0xe9:          //PCHL
            state->pc = (static_cast<uint16_t>(state->h) << 8) | state->l;
            break;
        case 0xea:          //JPE address
            if (state->cc.p == 1)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xeb: UnimplementedInstruction(state); break;
        case 0xec:          //CPE address
            if (state->cc.p == 1) {
                ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc+=2;
            break;
        case 0xed: UnimplementedInstruction(state); break;
        case 0xee: UnimplementedInstruction(state); break;
        case 0xef:          //RST 5
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 40;
            break;
        case 0xf0:          //RP address
            if (state->cc.s == 0)
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);

            state->pc+=2;
            break;
        case 0xf1: UnimplementedInstruction(state); break;
        case 0xf2:          //JP address
            if (state->cc.s == 0)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xf3: UnimplementedInstruction(state); break;
        case 0xf4:          //CP address
            if (state->cc.s == 0) {
                ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc+=2;
            break;
        case 0xf5: UnimplementedInstruction(state); break;
        case 0xf6: UnimplementedInstruction(state); break;
        case 0xf7:          //RST 6
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 48;
            break;
        case 0xf8:          //RM state
            if (state->cc.s == 1)
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);

            state->pc+=2;
            break;
        case 0xf9: UnimplementedInstruction(state); break;
        case 0xfa:          //JM address
            if (state->cc.s == 1)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc+=2;
            break;
        case 0xfb: UnimplementedInstruction(state); break;
        case 0xfc:          //CM address
            if (state->cc.s == 1) {
                ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xff;
                state->memory[state->sp-2] = (ret & 0xff);
                state->sp = state->sp - 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc+=2;
            break;
        case 0xfd:          //NO INSTRUCTION
            break;
        case 0xfe:          //CPI byte
            x = state->a - opcode[1];
            state->cc.z = (x == 0);
            state->cc.s = (0x80 == (x & 0x80));
            state->cc.p = parity(x);
            state->cc.cy = (state->a < opcode[1]);
            state->pc++;
            break;
        case 0xff:          //RST 7
            state->memory[state->sp - 1] = (state->pc >> 8) & 0xFF;
            state->memory[state->sp - 2] = state->pc & 0xFF;
            state->sp -= 2;
            state->pc = 56;
            break;
        default:
            std::cout << "Error" << std::endl;
            break;
    }

    state->pc++;

    return 0;
}

int dissasemble8080(unsigned char *codebuffer, int pc) {
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    std::cout << std::hex << std::setw(4) << std::setfill('0') << pc << " ";

    switch (*code) {
        case 0x00:
            printf("NOP");
            break;
        case 0x01:
            printf("LXI    B,#$%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x02:
            printf("STAX   B");
            break;
        case 0x03:
            printf("INX    B");
            break;
        case 0x04:
            printf("INR    B");
            break;
        case 0x05:
            printf("DCR    B");
            break;
        case 0x06:
            printf("MVI    B,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0x07:
            printf("RLC");
            break;
        case 0x08:
            printf("NULL");
            break;
        case 0x09:
            printf("DAD    B");
            break;
        case 0x0a:
            printf("LDAX   B");
            break;
        case 0x0b:
            printf("DCX    B");
            break;
        case 0x0c:
            printf("INR    C");
            break;
        case 0x0d:
            printf("DCR    C");
            break;
        case 0x0e:
            printf("MVI    C,#%02x", code[1]);
            opbytes  = 2;
            break;
        case 0x0f:
            printf("RRC");
            break;
        case 0x10:
            printf("NULL");
            break;
        case 0x11:
            printf("LXI    D,#%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x12:
            printf("STAX    D");
            break;
        case 0x13:
            printf("INX    D");
            break;
        case 0x14:
            printf("INR    D");
            break;
        case 0x15:
            printf("DCR    D");
            break;
        case 0x16:
            printf("MVI    D,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0x17:
            printf("RAL");
            break;
        case 0x18:
            printf("NULL");
            break;
        case 0x19:
            printf("DAD    D");
            break;
        case 0x1a:
            printf("LDAX    D");
            break;
        case 0x1b:
            printf("DCX    D");
            break;
        case 0x1c:
            printf("INR    E");
            break;
        case 0x1d:
            printf("DCR    E");
            break;
        case 0x1e:
            printf("MVI    E,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0x1f:
            printf("RAR");
            break;
        case 0x20:
            printf("NULL");
            break;
        case 0x21:
            printf("LXI    H,#%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x22:
            printf("SHLD   #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x23:
            printf("INX    H");
            break;
        case 0x24:
            printf("INR    H");
            break;
        case 0x25:
            printf("DCR    H");
            break;
        case 0x26:
            printf("MVI    H,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0x27:
            printf("DAA");
            break;
        case 0x28:
            printf("NULL");
            break;
        case 0x29:
            printf("DAD    H");
            break;
        case 0x2a:
            printf("LHLD   #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x2b:
            printf("DCX    H");
            break;
        case 0x2c:
            printf("INR    L");
            break;
        case 0x2d:
            printf("DCR    L");
            break;
        case 0x2e:
            printf("MVI    D,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0x2f:
            printf("CMA");
            break;
        case 0x30:
            printf("NULL");
            break;
        case 0x31:
            printf("LXI    SP,#%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x32:
            printf("STA    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x33:
            printf("INX    SP");
            break;
        case 0x34:
            printf("INR    M");
            break;
        case 0x35:
            printf("DCR    M");
            break;
        case 0x36:
            printf("MVI    M,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0x37:
            printf("STC");
            break;
        case 0x38:
            printf("NULL");
            break;
        case 0x39:
            printf("DAD    SP");
            break;
        case 0x3a:
            printf("LDA    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x3b:
            printf("DCX    SP");
            break;
        case 0x3c:
            printf("INR    A");
            break;
        case 0x3d:
            printf("DCR    A");
            break;
        case 0x3e:
            printf("MVI    M,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0x3f:
            printf("CMC");
            break;
        case 0x40:
            printf("MOV    B,B");
            break;
        case 0x41:
            printf("MOV    B,C");
            break;
        case 0x42:
            printf("MOV    B,D");
            break;
        case 0x43:
            printf("MOV    B,E");
            break;
        case 0x44:
            printf("MOV    B,H");
            break;
        case 0x45:
            printf("MOV    B,L");
            break;
        case 0x46:
            printf("MOV    B,M");
            break;
        case 0x47:
            printf("MOV    B,A");
            break;
        case 0x48:
            printf("MOV    C,B");
            break;
        case 0x49:
            printf("MOV    C,C");
            break;
        case 0x4a:
            printf("MOV    C,D");
            break;
        case 0x4b:
            printf("MOV    C,E");
            break;
        case 0x4c:
            printf("MOV    C,H");
            break;
        case 0x4d:
            printf("MOV    C,L");
            break;
        case 0x4e:
            printf("MOV    C,M");
            break;
        case 0x4f:
            printf("MOV    C,A");
            break;
        case 0x50:
            printf("MOV    D,B");
            break;
        case 0x51:
            printf("MOV    D,C");
            break;
        case 0x52:
            printf("MOV    D,D");
            break;
        case 0x53:
            printf("MOV    D,E");
            break;
        case 0x54:
            printf("MOV    D,H");
            break;
        case 0x55:
            printf("MOV    D,L");
            break;
        case 0x56:
            printf("MOV    D,M");
            break;
        case 0x57:
            printf("MOV    D,A");
            break;
        case 0x58:
            printf("MOV    E,B");
            break;
        case 0x59:
            printf("MOV    E,C");
            break;
        case 0x5a:
            printf("MOV    E,D");
            break;
        case 0x5b:
            printf("MOV    E,E");
            break;
        case 0x5c:
            printf("MOV    E,H");
            break;
        case 0x5d:
            printf("MOV    E,L");
            break;
        case 0x5e:
            printf("MOV    E,M");
            break;
        case 0x5f:
            printf("MOV    E,A");
            break;
        case 0x60:
            printf("MOV    H,B");
            break;
        case 0x61:
            printf("MOV    H,C");
            break;
        case 0x62:
            printf("MOV    H,D");
            break;
        case 0x63:
            printf("MOV    H,E");
            break;
        case 0x64:
            printf("MOV    H,H");
            break;
        case 0x65:
            printf("MOV    H,L");
            break;
        case 0x66:
            printf("MOV    H,M");
            break;
        case 0x67:
            printf("MOV    H,A");
            break;
        case 0x68:
            printf("MOV    L,B");
            break;
        case 0x69:
            printf("MOV    L,C");
            break;
        case 0x6a:
            printf("MOV    L,D");
            break;
        case 0x6b:
            printf("MOV    L,E");
            break;
        case 0x6c:
            printf("MOV    L,H");
            break;
        case 0x6d:
            printf("MOV    L,L");
            break;
        case 0x6e:
            printf("MOV    L,M");
            break;
        case 0x6f:
            printf("MOV    L,A");
            break;
        case 0x70:
            printf("MOV    M,B");
            break;
        case 0x71:
            printf("MOV    M,C");
            break;
        case 0x72:
            printf("MOV    M,D");
            break;
        case 0x73:
            printf("MOV    M,E");
            break;
        case 0x74:
            printf("MOV    M,H");
            break;
        case 0x75:
            printf("MOV    M,L");
            break;
        case 0x76:
            printf("HLT");
            break;
        case 0x77:
            printf("MOV    M,A");
            break;
        case 0x78:
            printf("MOV    A,B");
            break;
        case 0x79:
            printf("MOV    A,C");
            break;
        case 0x7a:
            printf("MOV    A,D");
            break;
        case 0x7b:
            printf("MOV    A,E");
            break;
        case 0x7c:
            printf("MOV    A,H");
            break;
        case 0x7d:
            printf("MOV    A,L");
            break;
        case 0x7e:
            printf("MOV    A,M");
            break;
        case 0x7f:
            printf("MOV    A,A");
            break;
        case 0x80:
            printf("ADD    B");
            break;
        case 0x81:
            printf("ADD    C");
            break;
        case 0x82:
            printf("ADD    D");
            break;
        case 0x83:
            printf("ADD    E");
            break;
        case 0x84:
            printf("ADD    H");
            break;
        case 0x85:
            printf("ADD    L");
            break;
        case 0x86:
            printf("ADD    M");
            break;
        case 0x87:
            printf("ADD    A");
            break;
        case 0x88:
            printf("ADC    B");
            break;
        case 0x89:
            printf("ADC    C");
            break;
        case 0x8a:
            printf("ADC    D");
            break;
        case 0x8b:
            printf("ADC    E");
            break;
        case 0x8c:
            printf("ADC    H");
            break;
        case 0x8d:
            printf("ADC    L");
            break;
        case 0x8e:
            printf("ADC    M");
            break;
        case 0x8f:
            printf("ADC    A");
            break;
        case 0x90:
            printf("SUB    B");
            break;
        case 0x91:
            printf("SUB    C");
            break;
        case 0x92:
            printf("SUB    D");
            break;
        case 0x93:
            printf("SUB    E");
            break;
        case 0x94:
            printf("SUB    H");
            break;
        case 0x95:
            printf("SUB    L");
            break;
        case 0x96:
            printf("SUB    M");
            break;
        case 0x97:
            printf("SUB    A");
            break;
        case 0x98:
            printf("SBB    B");
            break;
        case 0x99:
            printf("SBB    C");
            break;
        case 0x9a:
            printf("SBB    D");
            break;
        case 0x9b:
            printf("SBB    E");
            break;
        case 0x9c:
            printf("SBB    H");
            break;
        case 0x9d:
            printf("SBB    L");
            break;
        case 0x9e:
            printf("SBB    M");
            break;
        case 0x9f:
            printf("SBB    A");
            break;
        case 0xa0:
            printf("ANA    B");
            break;
        case 0xa1:
            printf("ANA    C");
            break;
        case 0xa2:
            printf("ANA    D");
            break;
        case 0xa3:
            printf("ANA    E");
            break;
        case 0xa4:
            printf("ANA    H");
            break;
        case 0xa5:
            printf("ANA    L");
            break;
        case 0xa6:
            printf("ANA    M");
            break;
        case 0xa7:
            printf("ANA    A");
            break;
        case 0xa8:
            printf("XRA    B");
            break;
        case 0xa9:
            printf("XRA    C");
            break;
        case 0xaa:
            printf("XRA    D");
            break;
        case 0xab:
            printf("XRA    E");
            break;
        case 0xac:
            printf("XRA    H");
            break;
        case 0xad:
            printf("XRA    L");
            break;
        case 0xae:
            printf("XRA    M");
            break;
        case 0xaf:
            printf("XRA    A");
            break;
        case 0xb0:
            printf("ORA    B");
            break;
        case 0xb1:
            printf("ORA    C");
            break;
        case 0xb2:
            printf("ORA    D");
            break;
        case 0xb3:
            printf("ORA    E");
            break;
        case 0xb4:
            printf("ORA    H");
            break;
        case 0xb5:
            printf("ORA    L");
            break;
        case 0xb6:
            printf("ORA    M");
            break;
        case 0xb7:
            printf("ORA    A");
            break;
        case 0xb8:
            printf("CMP    B");
            break;
        case 0xb9:
            printf("CMP    C");
            break;
        case 0xba:
            printf("CMP    D");
            break;
        case 0xbb:
            printf("ORA    E");
            break;
        case 0xbc:
            printf("ORA    H");
            break;
        case 0xbd:
            printf("ORA    L");
            break;
        case 0xbe:
            printf("ORA    M");
            break;
        case 0xbf:
            printf("ORA    A");
            break;
        case 0xc0:
            printf("RNZ");
            break;
        case 0xc1:
            printf("POP    B");
            break;
        case 0xc2:
            printf("JNZ    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xc3:
            printf("JMP    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xc4:
            printf("CNZ    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xc5:
            printf("PUSH   B");
            break;
        case 0xc6:
            printf("ADI    #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xc7:
            printf("RST    0");
            break;
        case 0xc8:
            printf("RZ");
            break;
        case 0xc9:
            printf("RET");
            break;
        case 0xca:
            printf("JZ     #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xcb:
            printf("NULL");
            break;
        case 0xcc:
            printf("CZ     #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xcd:
            printf("CALL   #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xce:
            printf("D8     #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xcf:
            printf("RST");
            break;
        case 0xd0:
            printf("RNC");
            break;
        case 0xd1:
            printf("POP    D");
            break;
        case 0xd2:
            printf("JNC    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xd3:
            printf("OUT    #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xd4:
            printf("CNC    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xd5:
            printf("PUSH   D");
            break;
        case 0xd6:
            printf("SUI    #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xd7:
            printf("RST    2");
            break;
        case 0xd8:
            printf("RC");
            break;
        case 0xd9:
            printf("NULL");
            break;
        case 0xda:
            printf("JC     #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xdb:
            printf("IN     #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xdc:
            printf("CC     #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xdd:
            printf("NULL");
            break;
        case 0xde:
            printf("SBI    #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xdf:
            printf("RST    3");
            break;
        case 0xe0:
            printf("RPO");
            break;
        case 0xe1:
            printf("POP    H");
            break;
        case 0xe2:
            printf("JPO    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xe3:
            printf("XTHL");
            break;
        case 0xe4:
            printf("CPO    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xe5:
            printf("PUSH   H");
            break;
        case 0xe6:
            printf("ANI    #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xe7:
            printf("RST    4");
            break;
        case 0xe8:
            printf("RPE");
            break;
        case 0xe9:
            printf("PCHL");
            break;
        case 0xea:
            printf("JPE    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xeb:
            printf("XCHG");
            break;
        case 0xec:
            printf("CPE    #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xed:
            printf("NULL");
            break;
        case 0xee:
            printf("XRI    #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xef:
            printf("RST    5");
            break;
        case 0xf0:
            printf("RP");
            break;
        case 0xf1:
            printf("POP    PSW");
            break;
        case 0xf2:
            printf("JP     #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xf3:
            printf("DI");
            break;
        case 0xf4:
            printf("CP      #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xf5:
            printf("PUSH   PSW");
            break;
        case 0xf6:
            printf("ORI     #%02x", code[1]);
            opbytes = 2;
            break;
        case 0xf7:
            printf("RST     6");
            break;
        case 0xf8:
            printf("RM");
            break;
        case 0xf9:
            printf("SPHL");
            break;
        case 0xfa:
            printf("JM     #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xfb:
            printf("EI");
            break;
        case 0xfc:
            printf("CM     #%02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xfd:
            printf("NULL");
            break;
        case 0xfe:
            printf("CPI    D8,#%02x", code[1]);
            opbytes = 2;
            break;
        case 0xff:
            printf("RST    7");
            break;
        default:
            printf("Error");
            break;
    }

    std::cout << std::endl;

    return opbytes;
}

int main(int argc, char* argv[])
{
    unsigned char *codebuffer;
    int pc = 0;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Could not open file " << filename << std::endl;
        return 1;
    }

    std::streampos fsize = file.tellg();
    file.seekg(0, std::ios::beg);

    codebuffer = new unsigned char[fsize];

    if (!file.read(reinterpret_cast<char*>(codebuffer), fsize))
    {
        std::cerr << "Error: Couldn't read the file " << filename << std::endl;
        delete[] codebuffer;
        return 1;
    }

    while (pc < fsize)
    {
        pc = pc + dissasemble8080(codebuffer, pc);
    }

    file.close();

    return 0;
}