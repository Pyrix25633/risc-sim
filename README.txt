╔═══════════════════════════════════════════════════════════════════════════════════════════════════╗
║  16bit Reduced Instruction Set Computer						                                    ║
╠═══════════════════════════════════════════════════════════════════════════════════════════════════╣
║  Composition																						║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐	║
║  │ Central Processing Unit																   	│	║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤	║
║  │ + Program Counter: 16bit register for the address of the next instruction					│	║
║  │ + Instruction Register: 16bit register for the current instruction							│	║
║  │ + Stack Pointer: 16bit register for the address of the next free location for the stack	│	║
║  │ + Address Register: 16bit register for the address to put on the address bus				│	║
║  │ + Data Register: 16bit register for the data to put on the data bus						│	║
║  │ + Status Register:																			│	║
║  │  - Z: 1bit register for zero																│	║
║  │  - N: 1bit register for negative															│	║
║  │  - C: 1bit register for carry											    				│	║
║  │  - V: 1bit register for overflow															│	║
║  │ + 16 (0 to F) general purpose 16bit registries												│	║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘	║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐	║
║  │ Central Memory																				│	║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤	║
║  │ + up to 2^16 8bit cells, 16bit data are stored in little endian							│	║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘	║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐	║
║  │ Input Output Devices																		│	║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤	║
║  │ + two IO Devices:											 								│	║
║  │  - Monitor: address 0x0000																	│	║
║  │  - Keyboard: address 0x0001																│	║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘	║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐	║
║  │ System Bus																					│	║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤	║
║  │ + Address Bus:	16bit										 								│	║
║  │ + Data Bus: 16bit																			│	║
║  │ + Control Bus:																				│	║
║  │  - Read: 1 = read, 0 = write																│	║
║  │  - Memory: 1 = memory, 0 = IO devices														│	║
║  │  - Word: 1 = word (16bit), 0 = byte (8bit)													│	║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘	║
╠═══════════════════════════════════════════════════════════════════════════════════════════════════╣
║  Instructions																						║
║  TODO     																						║
╠═══════════════════════════════════════════════════════════════════════════════════════════════════╣
║  Program Settings																					║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐	║
║  │ Interpreter																			   	│	║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤	║
║  │ + File: name of the file that contains	the program, .bin if binary or .hex if hexadecimal	│	║
║  │ + Ram Size: the dimension of the central memory, leave some space for the stack			│	║
║  │ + Start: the address where the program have to start										│	║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘	║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐	║
║  │ Window																					   	│	║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤	║
║  │ + FPS Counter: if FPS counter is active, true or false										│	║
║  │ + GUI Scale: the scale of the GUI, 0.5 to 3												│	║
║  │ + Scale: the scale for everything that is not a GUI, 0.5 to 3								│	║
║  │ + Height: the height of the window, change only if experiencing issues						│	║
║  │ + Width: the width of the window, change only if experiencing issues						│	║
║  │ + Max Framerate: render framerate cap														│	║
║  │ + Renderer Flags:																			│	║
║  │  - GPU Acceleration: if the renderer will use hardware acceleration					 	│	║
║  │  - Software Fallback: if the renderer will be a sofware fallback							│	║
║  │  - Target Texture: if the renderer will support rendering to texture						│	║
║  │  - Vsync: if the renderer will be synced with the monitor refresh rate 	                │	║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘	║
║  ┌────────────────────────────────────────────────────────────────────────────────────────────┐	║
║  │ Console    																			   	│	║
║  ├────────────────────────────────────────────────────────────────────────────────────────────┤	║
║  │ + File Log: if the program will log to file or to console                              	│	║
║  │ + Output color: if the console will be colored                                     		│	║
║  └────────────────────────────────────────────────────────────────────────────────────────────┘	║
╚═══════════════════════════════════════════════════════════════════════════════════════════════════╝
