# Intel 8080 Emulator

A test project of mine to learn about emulation development using the guide over at http://emulator101.com/.

This does not yet function at all. There are hundreds of these on the internet, so if you want to emulate the 8080, use one of those for now.

## Planned Features

- **Complete 8080 Instruction Set:** The emulator supports all 8080 instructions, including arithmetic, logical, control, and data transfer operations.
- **Stack Operations:** Proper implementation of stack operations including push, pop, and call/return mechanisms.
- **Interrupts and RST Handling:** Emulation of the 8080's interrupt system and the RST (Restart) instructions.
- **Memory Management:** Supports up to 64KB of memory, emulating the full address space of the Intel 8080.
- **Condition Codes:** Full support for the 8080's condition codes (flags), including zero, sign, parity, carry, and auxiliary carry.

## Getting Started

### Prerequisites

To compile and run this emulator, you'll need:

- Cmake (Optional technically)
- A C++ compiler supporting C++11 or later (e.g. `clang++`)

### Building the Emulator

1. Clone the repository:
    ```bash
    git clone https://github.com/PalindromicBreadLoaf/i8080-emu.git
    cd i8080-emu
    ```

2. Compile the emulator:
    ```bash
    clang++ -o 8080_emulator main.cpp
    ```

3. Run the emulator:
    ```bash
    ./8080_emulator
    ```
