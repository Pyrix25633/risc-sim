╔═══════════════════════════════════════════════════════════════════════════════════════════════════╗
║  16bit Reduced Instruction Set Computer                                                           ║
╠═══════════════════════════════════════════════════════════════════════════════════════════════════╣
║  Composition                                                                                      ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Central Processing Unit                                                                    │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + Program Counter: 16bit register for the address of the next instruction                  │   ║
║  │ + Instruction Register: 16bit register for the current instruction                         │   ║
║  │ + Stack Pointer: 16bit register for the address of the next free location for the stack    │   ║
║  │ + Address Register: 16bit register for the address to put on the address bus               │   ║
║  │ + Data Register: 16bit register for the data to put on the data bus                        │   ║
║  │ + Status Register:                                                                         │   ║
║  │  - Z: 1bit register for zero                                                               │   ║
║  │  - N: 1bit register for negative                                                           │   ║
║  │  - C: 1bit register for carry                                                              │   ║
║  │  - V: 1bit register for overflow                                                           │   ║
║  │ + 16 (0 to F) general purpose 16bit registries                                             │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Central Memory                                                                             │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + up to 2^16 8bit cells, 16bit data is stored in little endian                             │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Input Output Devices                                                                       │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + two IO Devices:                                                                          │   ║
║  │  - Monitor: address 0x0000                                                                 │   ║
║  │  - Keyboard: address 0x0001                                                                │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ System Bus                                                                                 │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + Address Bus: 16bit                                                                       │   ║
║  │ + Data Bus: 16bit                                                                          │   ║
║  │ + Control Bus:                                                                             │   ║
║  │  - Read: 1 = read, 0 = write                                                               │   ║
║  │  - Memory: 1 = memory, 0 = IO devices                                                      │   ║
║  │  - Word: 1 = word (16bit), 0 = byte (8bit)                                                 │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
╠═══════════════════════════════════════════════════════════════════════════════════════════════════╣
║  Instructions                                                                                     ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Data Transfer Group                                                                        │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + LDWI: loads a word, that is after the instruction, in "d"                                │   ║
║  │  - assembly: LDWI Rd hhhh                                                                  │   ║
║  │  - binary: 00010000dddd0000    hexadecimal: 10d0    status register: Z:D, N:D, C:-, V:-    │   ║
║  │ + LDWA: loads a word, whose address is after the instruction, in "d"                       │   ║
║  │  - assembly: LDWA Rd hhhh                                                                  │   ║
║  │  - binary: 00100000dddd0000    hexadecimal: 20d0    status register: Z:D, N:D, C:-, V:-    │   ║
║  │ + LDWR: loads a word, whose address is in the register "a", in "d"                         │   ║
║  │  - assembly: LDWA Rd Ra                                                                    │   ║
║  │  - binary: 00110000ddddaaaa    hexadecimal: 30da    status register: Z:D, N:D, C:-, V:-    │   ║
║  │ + LDBI: loads a byte, that is after the instruction, in "d"                                │   ║
║  │  - assembly: LDBI Rd hh                                                                    │   ║
║  │  - binary: 00010001dddd0000    hexadecimal: 11d0    status register: Z:D, N:-, C:-, V:-    │   ║
║  │ + LDBA: loads a byte, whose address is after the instruction, in "d"                       │   ║
║  │  - assembly: LDBA Rd hhhh                                                                  │   ║
║  │  - binary: 00100001dddd0000    hexadecimal: 21d0    status register: Z:D, N:-, C:-, V:-    │   ║
║  │ + LDBR: loads a byte, whose address is in the register "a", in "d"                         │   ║
║  │  - assembly: LDBR Rd Ra                                                                    │   ║
║  │  - binary: 00110001ddddaaaa    hexadecimal: 31da    status register: Z:D, N:-, C:-, V:-    │   ║
║  │ + STWA: stores a word, from "s", to an address which is after the instruction              │   ║
║  │  - assembly: STWA Rs hhhh                                                                  │   ║
║  │  - binary: 00100010ssss0000    hexadecimal: 22s0    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + STWR: stores a word, from "s", to an address which is in "a"                             │   ║
║  │  - assembly: STWR Rs Ra                                                                    │   ║
║  │  - binary: 00110010ssssaaaa    hexadecimal: 32sa    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + STBA: stores a byte, from "s", to an address which is after the instruction              │   ║
║  │  - assembly: STBA Rs hhhh                                                                  │   ║
║  │  - binary: 00100011ssss0000    hexadecimal: 23s0    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + STBR: stores a byte, from "s", to an address which is in "a"                             │   ║
║  │  - assembly: STBR Rs Ra                                                                    │   ║
║  │  - binary: 00110011ssssaaaa    hexadecimal: 33sa    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + CP or MV: copies the value of "s" in "d"                                                 │   ║
║  │  - assembly: CP Rs Rd or MV Rs Rd                                                          │   ║
║  │  - binary: 00000100ssssdddd    hexadecimal: 04sd    status register: Z:D, N:D, C:-, V:-    │   ║
║  │ + PUSH: saves the value of "s" in the stack                                                │   ║
║  │  - assembly: PUSH Rs                                                                       │   ║
║  │  - binary: 00001000ssss0000    hexadecimal: 08s0    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + POP: restores "d" from the stack                                                         │   ║
║  │  - assembly: POP Rd                                                                        │   ║
║  │  - binary: 00001001dddd0000    hexadecimal: 09d0    status register: Z:D, N:D, C:-, V:-    │   ║
║  │ + SPWR: writes the value of "s" in the stack pointer                                       │   ║
║  │  - assembly: SPWR Rs                                                                       │   ║
║  │  - binary: 00001110ssss0000    hexadecimal: 0Ds0    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + SPRD: writes the value of the stack pointer in "d"                                       │   ║
║  │  - assembly: SPRD Rd                                                                       │   ║
║  │  - binary: 00001101dddd0000    hexadecimal: 0Ed0    status register: Z:-, N:-, C:-, V:-    │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Arithmetic-Logic Group                                                                     │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + ADD: d = d + s                                                                           │   ║
║  │  - assembly: ADD Rs Rd                                                                     │   ║
║  │  - binary: 01000000ssssdddd    hexadecimal: 40sd    status register: Z:D, N:D, C:D, V:D    │   ║
║  │ + SUB: d = d - s                                                                           │   ║
║  │  - assembly: SUB Rs Rd                                                                     │   ║
║  │  - binary: 01000001ssssdddd    hexadecimal: 41sd    status register: Z:D, N:D, C:D, V:D    │   ║
║  │ + NOT: d = ~d                                                                              │   ║
║  │  - assembly: NOT Rd                                                                        │   ║
║  │  - binary: 01000010dddd0000    hexadecimal: 42d0    status register: Z:D, N:D, C:0, V:0    │   ║
║  │ + AND: d = d & s                                                                           │   ║
║  │  - assembly: AND Rs Rd                                                                     │   ║
║  │  - binary: 01000011ssssdddd    hexadecimal: 43sd    status register: Z:D, N:D, C:0, V:0    │   ║
║  │ + OR: d = d | s                                                                            │   ║
║  │  - assembly: OR Rs Rd                                                                      │   ║
║  │  - binary: 01000100ssssdddd    hexadecimal: 44sd    status register: Z:D, N:D, C:0, V:0    │   ║
║  │ + XOR: d = d ^ s                                                                           │   ║
║  │  - assembly: XOR Rs Rd                                                                     │   ║
║  │  - binary: 01000101ssssdddd    hexadecimal: 45sd    status register: Z:D, N:D, C:0, V:0    │   ║
║  │ + INC: d = d + 1                                                                           │   ║
║  │  - assembly: INC Rd                                                                        │   ║
║  │  - binary: 01001000dddd0000    hexadecimal: 48d0    status register: Z:D, N:D, C:D, V:D    │   ║
║  │ + DEC: d = d - 1                                                                           │   ║
║  │  - assembly: DEC Rd                                                                        │   ║
║  │  - binary: 01001001dddd0000    hexadecimal: 49d0    status register: Z:D, N:D, C:D, V:D    │   ║
║  │ + LSH: d = d << 1                                                                          │   ║
║  │  - assembly: LSH Rd                                                                        │   ║
║  │  - binary: 01001010dddd0000    hexadecimal: 4Ad0    status register: Z:D, N:D, C:D, V:0    │   ║
║  │ + RSH: d = d >> 1                                                                          │   ║
║  │  - assembly: RSH Rd                                                                        │   ║
║  │  - binary: 01001011dddd0000    hexadecimal: 4Bd0    status register: Z:D, N:D, C:0, V:0    │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Input-Output Group                                                                         │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + INB: to input a byte from keyboard, if it is 0 (Z = 1) no key was pressed                │   ║
║  │  - assembly: INB Rd hhhh                                                                   │   ║
║  │  - binary: 10000001dddd0000    hexadecimal: 81d0    status register: Z:D, N:-, C:-, V:-    │   ║
║  │ + OUTB: to output a byte to monitor                                                        │   ║
║  │  - assembly: OUTB Rs hhhh                                                                  │   ║
║  │  - binary: 10000011ssss0000    hexadecimal: 83s0    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ ! WARNING: I/O instructions have to be followed by the address (x16) of the I/O device     │   ║
║  │ ! \n (0x0A) will move all lines up by one, but will not move to a new line                 │   ║
║  │ ! \r (0x0D) will move to a new line, but will not move all lines up by one                 │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Control Group                                                                              │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + BR: writes an address, which is after the instruction, in the program counter            │   ║
║  │  - assembly: BR hhhh                                                                       │   ║
║  │  - binary: 1100000000000000    hexadecimal: C000    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + JMP: modifies the program counter with an offset "f"                                     │   ║
║  │  - assembly: JMP ff or JMP label                                                           │   ║
║  │  - binary: 11000001ffffffff    hexadecimal: C1ff    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + JMPZ: JMP if Z == 1                                                                      │   ║
║  │  - assembly: JMPZ ff or JMPZ label                                                         │   ║
║  │  - binary: 11000010ffffffff    hexadecimal: C2ff    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + JMPNZ: JMP if Z == 0                                                                     │   ║
║  │  - assembly: JMPNZ ff or JMPNZ label                                                       │   ║
║  │  - binary: 11000011ffffffff    hexadecimal: C3ff    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + JMPN: JMP if N == 1                                                                      │   ║
║  │  - assembly: JMPN ff or JMPN label                                                         │   ║
║  │  - binary: 11000100ffffffff    hexadecimal: C4ff    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + JMPNN: JMP if N == 0                                                                     │   ║
║  │  - assembly: JMPNN ff or JMPNN label                                                       │   ║
║  │  - binary: 11000101ffffffff    hexadecimal: C5ff    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + JMPC: JMP if C == 1                                                                      │   ║
║  │  - assembly: JMPC ff or JMPC label                                                         │   ║
║  │  - binary: 11000110ffffffff    hexadecimal: C6ff    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + JMPV: JMP if V == 1                                                                      │   ║
║  │  - assembly: JMPV ff or JMPV label                                                         │   ║
║  │  - binary: 11000111ffffffff    hexadecimal: C7ff    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + CALL: starts a subroutine, from an address which is after the instruction                │   ║
║  │  - assembly: CALL hhhh                                                                     │   ║
║  │  - binary: 1100100000000000    hexadecimal: C800    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + RET: returns from the subroutine, where it was called                                    │   ║
║  │  - assembly: RET                                                                           │   ║
║  │  - binary: 1100100100000000    hexadecimal: C900    status register: Z:-, N:-, C:-, V:-    │   ║
║  │ + HLT: exit the program                                                                    │   ║
║  │  - assembly: HLT                                                                           │   ║
║  │  - binary: 1100111100000000    hexadecimal: CF00    status register: Z:-, N:-, C:-, V:-    │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Legend                                                                                     │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + s: source register                                                                       │   ║
║  │ + d: destination register                                                                  │   ║
║  │ + a: register containing the address                                                       │   ║
║  │ + f: offset                                                                                │   ║
║  │ + h: hexadecimal digit, if address or offset can be a label                                │   ║
║  │ + D: depends                                                                               │   ║
║  │ + -: unaffected                                                                            │   ║
║  │ + 0: setted to 0                                                                           │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
╠═══════════════════════════════════════════════════════════════════════════════════════════════════╣
║  Program Settings                                                                                 ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Interpreter                                                                                │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + File: name of the file that contains the program                                         │   ║
║  │  - .bin if binary                                                                          │   ║
║  │  - .hex if hexadecimal                                                                     │   ║
║  │  - .asm if assembly                                                                        │   ║
║  │ + Ram Size: the dimension of the central memory, leave some space for the stack            │   ║
║  │ + Start: the address where the program has to start                                        │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Window                                                                                     │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + FPS Counter: if FPS counter is active, true or false                                     │   ║
║  │ + Scale: the scale of the GUI, 0.5 to 3                                                    │   ║
║  │ + Height: the height of the window, change only if experiencing issues                     │   ║
║  │ + Width: the width of the window, change only if experiencing issues                       │   ║
║  │ + Max Framerate: render framerate cap                                                      │   ║
║  │ + Renderer Flags:                                                                          │   ║
║  │  - GPU Acceleration: if the renderer will use hardware acceleration                        │   ║
║  │  - Software Fallback: if the renderer will be a sofware fallback                           │   ║
║  │  - Target Texture: if the renderer will support rendering to texture                       │   ║
║  │  - Vsync: if the renderer will be synced with the monitor refresh rate                     │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐   ║
║  │ Console                                                                                    │   ║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤   ║
║  │ + File Log: if the program will log to file or to console                                  │   ║
║  │ + Output Color: if the console will be colored                                             │   ║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘   ║
╚═══════════════════════════════════════════════════════════════════════════════════════════════════╝